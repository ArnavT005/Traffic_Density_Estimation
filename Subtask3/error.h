void baseline(cv::VideoCapture video, cv::Mat background, cv::Mat matrix, ofstream &file) {
    video.set(cv::CAP_PROP_POS_MSEC, 0);

    cv::Mat frame1, frame2, thresh;

    // total image area
    float AREA = background.size().area();
    float denseQ = 0, denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);
    file<<"Time,DenseQueue,DenseMove\n";

    while (true) {

        //read Frame 2
        for (int i = 0; i < see_every_n_frame; i++) {
            video.read(frame2);
            frame++;
        }

        //Video End
        if (frame2.empty()) {
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
        time = (float)frame / 15;

        file << time << "," << denseQ << "," << denseM << "\n";

        //update frame1 to frame2 and loop back
        frame1 = frame2;
    }
}

vector<float> error(ifstream& base, ifstream& file, string move, string queue, int x = 3) {
    vector<float> error = {0, 0};
    ofstream fmove(move), fqueue(queue);
    int baseLineNumber = 1, fileLineNumber, currentLine = 1;
    string buffer, baseQ, baseM, fileQ, fileM;
    bool flag = false;
    getline(base, buffer);
    getline(file, buffer);
    fmove<<"Time, Output, Baseline\n";
    fqueue<<"Time, Output, Baseline\n";

    getline(file, buffer, ',');
    getline(file, fileQ, ',');
    getline(file, fileM);
    
    while (getline(base, buffer, ',')) {
        getline(base, baseQ, ',');
        getline(base, baseM);
        fileLineNumber = (3 * baseLineNumber) / x;
        if (fileLineNumber == 0) {
            error[0] += (stof(baseQ)) * (stof(baseQ));
            error[1] += (stof(baseM)) * (stof(baseM));
            fmove<<buffer<<",0,"<<baseM<<"\n";
            fqueue<<buffer<<",0,"<<baseQ<<"\n";
            baseLineNumber++;
            continue;
        }
        while (!flag && currentLine < fileLineNumber) {
            if (!getline(file, buffer, ',')) {
                flag = true;
                break;
            }
            getline(file, fileQ, ',');
            getline(file, fileM);
            currentLine++;
        }
        //cout<<fileQ;
        error[0] += (stof(baseQ) - stof(fileQ)) * (stof(baseQ) - stof(fileQ));
        error[1] += (stof(baseM) - stof(fileM)) * (stof(baseM) - stof(fileM));
        fmove<<buffer<<","<<fileM<<","<<baseM<<"\n";
        fqueue<<buffer<<","<<fileQ<<","<<baseQ<<"\n";
        baseLineNumber++;
    }
    error[0] /= (baseLineNumber - 1);
    error[0] = sqrt(error[0]);
    error[1] /= (baseLineNumber - 1);
    error[1] = sqrt(error[1]);
    fmove.close();
    fqueue.close();
    return error;
}
