#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

// Global variables
Mat img, resizedImg;
Rect selectedRect;
bool drawing = false;
const int windowWidth = 800;
const int windowHeight = 600;
double scale;

void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    static Point startPoint;
    if (event == EVENT_LBUTTONDOWN) {
        drawing = true;
        startPoint = Point(x / scale, y / scale);
    } else if (event == EVENT_MOUSEMOVE && drawing) {
        resizedImg = img.clone();
        rectangle(resizedImg, startPoint, Point(x / scale, y / scale), Scalar(0, 255, 0), 2);
        Mat displayImg;
        resize(resizedImg, displayImg, Size(), scale, scale);
        imshow("Image", displayImg);
    } else if (event == EVENT_LBUTTONUP) {
        drawing = false;
        selectedRect = Rect(startPoint, Point(x / scale, y / scale));
        printf("Selected Frame Size: %d x %d\n", selectedRect.width, selectedRect.height);
    }
}

int main() {
    img = imread("wall.jpg");
    if (img.empty()) {
        printf("Could not open image.\n");
        return -1;
    }
    
    // Resize image cause it messes up screen otherwise
    scale = min((double)windowWidth / img.cols, (double)windowHeight / img.rows);
    resize(img, resizedImg, Size(), scale, scale);

    printf("Select frame area and press ENTER: ");
    
    namedWindow("Image", WINDOW_AUTOSIZE);
    setMouseCallback("Image", mouseCallback);
    imshow("Image", resizedImg);
    waitKey(0);
    
    int numFrames;
    printf("Enter number of frames: ");
    scanf("%d", &numFrames);
    
    int endX;
    printf("Enter endpoint X coordinate: ");
    scanf("%d", &endX);
    endX = endX / scale;
    
    int spacing = (endX - selectedRect.x - numFrames * selectedRect.width) / (numFrames - 1);
    
    Mat resultImg = img.clone();
    Mat overlay = img.clone();
    
    for (int i = 0; i < numFrames; i++) {
        int x = selectedRect.x + i * (selectedRect.width + spacing);
        
        // Draw frame boundaries
        rectangle(overlay, Point(x, selectedRect.y), Point(x + selectedRect.width, selectedRect.y + selectedRect.height), Scalar(255, 0, 0), -1);
        
        rectangle(resultImg, Point(x, selectedRect.y), Point(x + selectedRect.width, selectedRect.y + selectedRect.height), Scalar(0, 0, 255), 4);
    }
    
    // Blend overlay into origianl image
    double alpha = 0.4;
    addWeighted(overlay, alpha, resultImg, 1 - alpha, 0, resultImg);
    
    Mat displayResult;
    resize(resultImg, displayResult, Size(), scale, scale);
    imshow("Result", displayResult);
    waitKey(0);
    
    return 0;
}
