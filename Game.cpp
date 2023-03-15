#include "Game.hpp"

static const char *colors[7] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/

Game::Game(game_params param) {
    this->filename = param.filename;
    this->first_thread_num = param.n_thread;
    this->m_thread_num = param.n_thread;
    this->m_gen_num = param.n_gen;
    this->interactive_on = param.interactive_on;
    this->print_on = param.print_on;
    matrixField1 = new vector<vector<uint>>();
    matrixField2 = new vector<vector<uint>>();
    jobsQueue = new PCQueue<Job*>();
    pthread_mutex_init(&mutexCounter,NULL);
    pthread_mutex_init(&mutexTimer,NULL);
}

Game::~Game() {
    delete matrixField1;
    delete matrixField2;
    delete jobsQueue;
}

const vector<double> Game::gen_hist() const {
    return m_gen_hist;
}
const vector<double> Game::tile_hist() const {
    return m_tile_hist;
}
uint Game::thread_num() const {
    return m_thread_num;
}

PCQueue<Job *>* Game::getJobQueue() {
    return jobsQueue;
}

int_mat * Game::getCurrField() {
    return currField;
}

int_mat * Game::getNextField() {
    return nextField;
}

vector<double> * Game::getTileHist() {
    return &m_tile_hist;
}

int Game::getNeighborsNum(uint i, uint j, int *dominant, double * sum) {
    uint Histogram[8] = {0};
    int counter = 0;
    bool up=false, down=false;
    if(i!=0) {
        if((*currField)[i-1][j] !=0) {
            counter++;
            Histogram[(*currField)[i-1][j]]++;
            *sum+=((*currField)[i-1][j]);
        }
        up=true;
    }
    if(i<(*currField).size()-1) {
        if((*currField)[i+1][j] !=0) {
            counter++;
            Histogram[(*currField)[i+1][j]]++;
            *sum+=((*currField)[i+1][j]);
        }
        down=true;
    }
    if(j!=0) {
        if(up) {
            if((*currField)[i-1][j-1] !=0) {
                counter++;
                Histogram[(*currField)[i-1][j-1]]++;
                *sum+=((*currField)[i-1][j-1]);
            }
        }
        if((*currField)[i][j-1] !=0) {
            counter++;
            Histogram[(*currField)[i][j-1]]++;
            *sum+=((*currField)[i][j-1]);
        }
        if(down) {
            if((*currField)[i+1][j-1] !=0) {
                counter++;
                Histogram[(*currField)[i+1][j-1]]++;
                *sum+=((*currField)[i+1][j-1]);
            }
        }
    }
    if(j<(*currField)[i].size()-1) {
        if(up) {
            if((*currField)[i-1][j+1] !=0) {
                counter++;
                Histogram[(*currField)[i-1][j+1]]++;
                *sum+=((*currField)[i-1][j+1]);
            }
        }
        if((*currField)[i][j+1] !=0) {
            counter++;
            Histogram[(*currField)[i][j+1]]++;
            *sum+=((*currField)[i][j+1]);
        }
        if(down) {
            if((*currField)[i+1][j+1] !=0) {
                counter++;
                Histogram[(*currField)[i+1][j+1]]++;
                *sum+=((*currField)[i+1][j+1]);
            }
        }
    }
    uint max=0,domColor=0;
    for(uint k=1;k<=7;k++) {
        if((Histogram[k]*k)>max) {
            max=(Histogram[k]*k);
            domColor = k;
        } else if((Histogram[k]*k)==max){
            if(k<domColor) {
                domColor = k;
            }
        }
    }
    *dominant = domColor;
    return counter;
}

void Game::phase1(Job* job) {
    auto tile_start = std::chrono::system_clock::now();
    int dom=0,neighborsNum;
    double sum=0;
    for(uint i=job->startIndex;i<=job->endIndex;i++) {
        for(uint j=0; j<(*currField)[i].size(); j++) {
            neighborsNum = getNeighborsNum(i,j,&dom,&sum);
            sum=0;
            if((*currField)[i][j] == 0) {
                if(neighborsNum==3){
                    (*nextField)[i][j]=dom;
                } else {
                    (*nextField)[i][j]=0;
                }
            } else {
                if(neighborsNum==2 ||neighborsNum==3){
                    (*nextField)[i][j]=(*currField)[i][j];
                } else {
                    (*nextField)[i][j]=0;
                }
            }
        }
    }
    auto tile_end = std::chrono::system_clock::now();
    pthread_mutex_lock(&mutexTimer);
    m_tile_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(tile_end - tile_start).count());
    pthread_mutex_unlock(&mutexTimer);
}

void Game::phase2(Job* job) {
    auto tile_start = std::chrono::system_clock::now();
    int dom=0;
    double sum=0;
    int neighborsNum;
    for(uint i=job->startIndex;i<=job->endIndex;i++) {
        for(uint j=0; j<(*currField)[i].size(); j++) {
            neighborsNum = getNeighborsNum(i,j,&dom,&sum);
            if((*currField)[i][j] != 0) {
                sum+=((*currField)[i][j]);
                double avg = sum/(neighborsNum+1);
                (*nextField)[i][j] = round(avg);
            } else {
                (*nextField)[i][j] = 0;
            }
            sum=0;
        }
    }
    auto tile_end = std::chrono::system_clock::now();
    pthread_mutex_lock(&mutexTimer);
    m_tile_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(tile_end - tile_start).count());
    pthread_mutex_unlock(&mutexTimer);
}

void Game::run() {
	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(nullptr);
	} // generation loop
    jobInsert(KILL);
    while(jobsCounter!=0){

    }
	print_board("Final Board");
	_destroy_game();
}

void Game::_init_game() {
    vector<string> matrixRows = utils::read_lines(this->filename);
    uint matrixHeight = matrixRows.size();
    if(matrixHeight < this->m_thread_num) {
        this->m_thread_num = matrixHeight;
    }
    vector<string> matRow = utils::split(matrixRows[0], DEF_MAT_DELIMITER);
    uint matrixWidth = matRow.size();
    // Create game fields - Consider using utils:read_file, utils::split
    for(uint i=0; i<matrixHeight; i++) {
        vector<uint>* v1 = new vector<uint>();
        matrixField1->push_back(*v1);
        vector<uint>* v2= new vector<uint>();
        matrixField2->push_back(*v2);
        matRow = utils::split(matrixRows[i], DEF_MAT_DELIMITER);
        for(uint j=0; j<matrixWidth; j++) {
            (*matrixField1)[i].push_back(stoi(matRow[j]));
            (*matrixField2)[i].push_back(0);
        }
    }
    currField = matrixField1;
    nextField = matrixField2;
    jobsCounter = 0;
	// Create & Start threads
    for (unsigned int i=0; i<m_thread_num; i++) {
        Thread* thread = new subThread(i,this);
        m_threadpool.push_back(thread);
        m_threadpool[i]->start();
    }
    // Testing of your implementation will presume all threads are started here
}
void Game::jobDone() {
    pthread_mutex_lock(&(mutexCounter));
    jobsCounter--; 
    pthread_mutex_unlock(&(mutexCounter));
}
void Game::jobInsert(Phase phase) {
    uint j=0;
    int height = matrixField1->size();
    uint jobRows = height/m_thread_num;
    uint remain = height%m_thread_num;
    for(uint i=0; i<m_thread_num; i++) {
        Job* newJob= new Job();
        newJob->startIndex = j;
        if(i+1==m_thread_num && remain != 0 ) {
            newJob->endIndex = j+jobRows+remain-1;
        }else {
            newJob->endIndex = j+jobRows-1;
        }
        newJob->phase = phase;
        j+=jobRows;
        pthread_mutex_lock(&(mutexCounter));
        jobsCounter++; 
        jobsQueue->push(newJob);
        pthread_mutex_unlock(&(mutexCounter));
    }
}

void Game::_step(uint curr_gen) {
    // Push jobs to queue
    jobInsert(PHASE1);
    while(jobsCounter!=0); 
    // Wait for the workers to finish calculating
    int_mat* tempPointer;
    tempPointer = currField;
    currField = nextField;
    nextField = tempPointer;
    //print_board("myTest");
    jobInsert(PHASE2);
    while(jobsCounter!=0); 
    tempPointer = currField;
    currField = nextField;
    nextField = tempPointer;
	// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade 
}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here
	for (uint i = 0; i < m_thread_num; ++i) {
        m_threadpool[i]->join();
    }
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {
    uint field_height = currField->size();
    uint field_width = (*currField)[0].size();
	if(print_on){ 

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;

        cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
        for (uint i = 0; i < field_height; ++i) {
            cout << u8"║";
            for (uint j = 0; j < field_width; ++j) {
                if ((*currField)[i][j] > 0)
                    cout << colors[(*currField)[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";
            }
            cout << u8"║" << endl;
        }
        cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}


/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters. 

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
                if (field[i][j] > 0)
                    cout << colors[field[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/ 



