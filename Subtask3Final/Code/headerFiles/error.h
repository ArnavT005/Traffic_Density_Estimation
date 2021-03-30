vector<float> error(ifstream& base, ifstream& file, string move, string queue, int x = 3) {
    vector<float> error = {0, 0};
    ofstream fmove(move), fqueue(queue);
    int baseLineNumber = 1, fileLineNumber, currentLine = 1;
    string buffer, baseQ, baseM, fileQ, fileM;
    bool flag = false;
    getline(base, buffer);
    getline(file, buffer);
    fmove<<"Time, Moving Output, Moving Baseline\n";
    fqueue<<"Time, Queue Output, Queue Baseline\n";

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
