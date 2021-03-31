void M2_sparseDense(VideoCapture video, Mat background, Mat matrix, int x, ofstream &file) {
    video.set(cv::CAP_PROP_POS_MSEC, 0);
    Mat frame1, frame2;
    float AREA = background.size().area();
    float denseM = 0, time;
    int see_every_n_frame = 3, frame = 1;

    //read Frame 1
    video.read(frame1);
    frame1 = warpAndCrop(frame1, matrix);
    cvtColor(frame1, frame1, COLOR_BGR2GRAY);
    file << "Time,Queue Density,Moving Density\n";

    if (x == 0) {
        while (true) {

            for (int i = 0; i < see_every_n_frame; i++) {
                video.read(frame2);
                frame++;
            }

             //Video End
            if (frame2.empty()) {
                break;
            }

            frame2 = warpAndCrop(frame2, matrix);

            cvtColor(frame2, frame2, COLOR_BGR2GRAY);

            Mat opt_flow(frame1.size(), CV_32FC2);

            calcOpticalFlowFarneback(frame1, frame2, flow, 0.5, 3, 15, 3, 7, 1.5, 0);

            Mat realImg[2];
            split(flow, realImg);

            Mat vec_mag, vec_ang, magn_norm;
            cartToPolar(realImg[0], realImg[1], vec_mag, vec_ang, true);
            normalize(vec_mag, magn_norm, 0.0, 1.0, NORM_MINMAX);
            angle *= (1.f/510.f);

            Mat hsv_parts[3], hsv, hsv_scaled, bgr;
            hsv_parts[0] = angle; // hue
            hsv_parts[1] = Mat::ones(angle.size(), CV_32F); // saturation
            hsv_parts[2] = magn_norm; // value
            merge(hsv_parts, 3, hsv);
            hsv.convertTo(hsv_scaled, CV_8U, 255.0);

            cvtColor(hsv8, bgr, COLOR_HSV2BGR);
            cvtColor(bgr, bgr, COLOR_BGR2GRAY);

            threshold(bgr, bgr, 15, 255, THRESH_BINARY);

            Mat kernel7x7 = getStructuringElement(MORPH_RECT, Size(7, 7));
            Mat kernel15x15 = getStructuringElement(MORPH_RECT, Size(15, 15));

            erode(bgr, bgr, kernel7x7);
            dilate(bgr, bgr, kernel15x15);

            denseM = findArea(bgr) / AREA;

            // video is 15 FPS
            time = (float)frame / 15;

            file << time << "," << 0.0 << "," << denseM << "\n";

            //update frame1 to frame2 and loop back
            frame1 = frame2;
        }
    }
}
