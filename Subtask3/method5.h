struct m5{
    cv::VideoCapture video;
    cv::Mat background;
    cv::Mat matrix;
    string file;
    int start;
    int end;
    int magic_number;
};

void* M5(void* arg) {
    m5* args = (m5*)arg;
    cv::VideoCapture video = args-> video;
    cv::Mat background = args-> background;
    cv::Mat matrix = args-> matrix;
    ofstream file(args-> file);
    int start = args-> start;
    int magic = args -> magic_number;
    int end = args -> end;
    //start = start + (3 - start%3)%3;
    // MAGIC
    //if (start!=0) { start-=2; }
    start -= magic;
    
    video.set(cv::CAP_PROP_POS_FRAMES, start);
    cout<<start<<" "<<end<<"\n";

    cv::Mat frame1, frame2, thresh;

    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = start + 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);

    while (frame <= end) {

        //read Frame 2
        for (int i = 0; i < see_every_n_frame; i++) {
            video.read(frame2);
            frame++;
        }

        //Video End
        //MAGIC
        if (frame2.empty() || frame > end + 1) {
        //if (frame2.empty() || frame > end) {
            break;
        }

        frame2 = warpAndCrop(frame2, matrix);

        //Queue Density - subtract background
        thresh = subImg(background.clone(), frame2.clone(), 40);
        denseQ = findArea(thresh) / AREA;

        //Dynamic Density - subtract frame1
        thresh = subImg(frame1.clone(), frame2.clone());
        denseM = findArea(thresh) / AREA;

        // error correction if Queue density < Dynamic density
        // slight error occurs due to different threshold values used
        // when only moving vehicles are present
        denseQ = denseQ > denseM ? denseQ : denseM;

        // video is 15 FPS
        //MAGIC
        //if(start!=0){time = (float)(frame+2)/15; }
        //else{ time = (float)frame / 15;}
        time = (float)(frame+magic) / 15;

        file << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
    file.close();
    return NULL;
}

void M5_temporalSplit(string path, cv::Mat background, cv::Mat matrix, int x, ofstream &file) {
    pthread_t t[8];
    m5 arg[8];
    ifstream f[8];
    vector<vector<int>> magicVector = { {0}, {0,1}, {0,2,2}, {0,2,1,2}, {0,2,1,1,2}, {0,1,2,1,2,0}, {0,1,2,1,1,1,0}, {0,2,2,2,1,0,2,1} };
    cv::VideoCapture temp(path);
    int frame_cnt = temp.get(cv::CAP_PROP_FRAME_COUNT);
    int start = 0, end = frame_cnt/x;
    end = end + (3-end%3)%3;
    file<<"Time,DenseQueue,DenseMove\n";
    for(int i = 0; i < x; i++)
    {
        arg[i].video.open(path);
        arg[i].background = background;
        arg[i].matrix = matrix;
        arg[i].file = to_string(i+1) + ".txt";
        arg[i].start = start;
        arg[i].end = end;
        arg[i].magic_number = magicVector[x-1][i];
        pthread_create(&t[i], NULL, &M5, (void*)(&arg[i]));
        start = end;
        end = ((i + 2) * frame_cnt) / x;
        end = end + (3 - end % 3) % 3;
    }
    for(int i = 0; i < x; i++)
    {
        pthread_join(t[i], NULL);
        f[i].open(arg[i].file);
    }
    for(int i = 0; i < x; i++){
        string buffer;
        while(getline(f[i], buffer)){
            file<<buffer<<"\n";
        }
    }
    remove("1.txt");
    remove("2.txt");
    remove("3.txt");
    remove("4.txt");
    remove("5.txt");
    remove("6.txt");
    remove("7.txt");
    remove("8.txt");
}
