
// EXTRA CREDIT -
//void M2_sparseDense(VideoCapture video, Mat background, Mat matrix, int x = 0) {
    // Mat frame1, frame2, thresh;
    // float AREA = background.size().area();
    // float denseQ = 0, denseM = 0, time;
    // int see_every_n_frame = 3, frame = 1;

    // //read Frame 1
    // video.read(frame1);
    // frame1 = warpAndCrop(frame1, matrix);
    // cvtColor(frame1, frame1, COLOR_BGR2GRAY);

    // if (x == 0) {
    //     while (true) {

    //         for (int i = 0; i < see_every_n_frame; i++) {
    //             video.read(frame2);
    //             frame++;
    //         }

    //         //Video End
    //         if (frame2.empty()) {
    //             break;
    //         }

    //         frame2 = warpAndCrop(frame2, matrix);

    //         thresh = subImg(background.clone(), frame2.clone(), 40);
    //         denseQ = findArea(thresh) / AREA;

    //         cvtColor(frame2, frame2, COLOR_BGR2GRAY);

    //         Mat flow(frame1.size(), CV_32FC2);

    //         calcOpticalFlowFarneback(frame1, frame2, flow, 0.5, 3, 15, 3, 7, 1.5, 0);

    //         Mat flow_parts[2];
    //         split(flow, flow_parts);

    //         Mat magnitude, angle, magn_norm;
    //         cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
    //         normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
    //         angle *= ((1.f / 360.f) * (180.f / 255.f));

    //         Mat _hsv[3], hsv, hsv8, bgr;
    //         _hsv[0] = angle;
    //         _hsv[1] = Mat::ones(angle.size(), CV_32F);
    //         _hsv[2] = magn_norm;
    //         merge(_hsv, 3, hsv);
    //         hsv.convertTo(hsv8, CV_8U, 255.0);

    //         cvtColor(hsv8, bgr, COLOR_HSV2BGR);
    //         cvtColor(bgr, bgr, COLOR_BGR2GRAY);

    //         threshold(bgr, bgr, 15, 255, THRESH_BINARY);

    //         Mat kernel7x7 = getStructuringElement(MORPH_RECT, Size(7, 7));
    //         Mat kernel15x15 = getStructuringElement(MORPH_RECT, Size(15, 15));

    //         erode(bgr, bgr, kernel7x7);
    //         dilate(bgr, bgr, kernel15x15);

    //         denseM = findArea(bgr) / AREA;

    //         // error correction if Queue density < Dynamic density
    //         // slight error occurs due to different threshold values used
    //         // when only moving vehicles are present
    //         denseQ = denseQ > denseM ? denseQ : denseM;

    //         // video is 15 FPS
    //         time = (float)frame / 15;

    //         //f2 << time << "," << denseQ << "," << denseM << "\n";

    //         //update frame1 to frame2 and loop back
    //         frame1 = frame2;
    //     }
    // }
    // else {
    //     vector<Scalar> colors;
    //     RNG rng;
    //     for (int i = 0; i < 100; i++) {
    //         int r = rng.uniform(0, 256);
    //         int g = rng.uniform(0, 256);
    //         int b = rng.uniform(0, 256);
    //         colors.push_back(Scalar(r, g, b));
    //     }
    //     vector<Point2f> p0, p1;
    //     /*goodFeaturesToTrack(frame1, p0, 100, 0.3, 7, Mat(), 7, false, 0.04);
    //     Mat mask = Mat::zeros(frame1.size(), frame1.type());*/
    //     while (true) {

    //         for (int i = 0; i < see_every_n_frame; i++) {
    //             video.read(frame2);
    //             frame++;
    //         }

    //         //Video End
    //         if (frame2.empty()) {
    //             break;
    //         }

    //         goodFeaturesToTrack(frame1, p0, 100, 0.3, 7, Mat(), 7, false, 0.04);
    //         Mat mask = Mat::zeros(frame1.size(), frame1.type());

    //         frame2 = warpAndCrop(frame2, matrix);

    //         thresh = subImg(background.clone(), frame2.clone(), 40);
    //         denseQ = findArea(thresh) / AREA;

    //         cvtColor(frame2, frame2, COLOR_BGR2GRAY);

    //         vector<uchar> status;
    //         vector<float> err;

    //         TermCriteria criteria = TermCriteria((TermCriteria::COUNT)+(TermCriteria::EPS), 10, 0.03);
    //         calcOpticalFlowPyrLK(frame1, frame2, p0, p1, status, err, Size(15, 15), 2, criteria);

    //         vector<Point2f> good_new;

    //         for (uint i = 0; i < p0.size(); i++) {
    //             if (status[i] == 1) {
    //                 good_new.push_back(p1[i]);
    //                 line(mask, p1[i], p0[i], colors[i], 2);
    //                 circle(frame2, p1[i], 5, colors[i], -1);
    //             }
    //         }

    //         Mat img;
    //         add(frame2, mask, img);

    //         imshow("Flow", img);
    //         waitKey(30);

    //         //denseM = findArea(bgr) / AREA;

    //         // error correction if Queue density < Dynamic density
    //         // slight error occurs due to different threshold values used
    //         // when only moving vehicles are present
    //         denseQ = denseQ > denseM ? denseQ : denseM;

    //         // video is 15 FPS
    //         time = (float)frame / 15;

    //         //f2 << time << "," << denseQ << "," << denseM << "\n";

    //         //update frame1 to frame2 and loop back
    //         frame1 = frame2.clone();
    //         p0 = good_new;

    //     }


    // }
//}
