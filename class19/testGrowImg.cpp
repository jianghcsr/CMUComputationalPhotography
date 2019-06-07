#include "testGrowImg.h"

using cv::Mat;
using cv::Range;
using std::list;
using std::vector;

int main(int argc, char **argv) {
  testGetUnfilledNeighbors();
  testFindMatches();
  testGrowImg2();
  return 0;
}

void stichImg(const Mat &imgSrc, const Mat &imgDst, const Mat &dstMask,
              Mat &outputImg) {
  int h = fmax(imgSrc.rows, imgDst.rows);
  int w = fmax(imgSrc.cols, imgDst.cols);

  outputImg = Mat::zeros(h, 3 * w, imgSrc.type());

  int imgSrcStartY = (h - imgSrc.rows) / 2.0f;
  int imgSrcEndY = (h + imgSrc.rows) / 2.0f;
  int imgSrcStartX = (w - imgSrc.cols) / 2.0f;
  int imgSrcEndX = imgSrcStartX + imgSrc.cols;
  imgSrc.copyTo(outputImg(Range(imgSrcStartY, imgSrcEndY),
                          Range(imgSrcStartX, imgSrcEndX)));

  int imgDstStartY = (h - imgDst.rows) / 2.0f;
  int imgDstEndY = (h + imgDst.rows) / 2.0f;
  int imgDstStartX = w + (w - imgDst.cols) / 2.0f;
  int imgDstEndX = w + (w - imgDst.cols) / 2.0f + imgDst.cols;
  imgDst.copyTo(outputImg(Range(imgDstStartY, imgDstEndY),
                          Range(imgDstStartX, imgDstEndX)));

  int dstMaskStartY = (h - dstMask.rows) / 2.0f;
  int dstMaskEndY = (h + dstMask.rows) / 2.0f;
  int dstMaskStartX = w * 2+ (w - dstMask.cols) / 2.0f;
  int dstMaskEndX = w * 2 + (w - dstMask.cols) / 2.0f + dstMask.cols;
  Mat dstMaskShow = dstMask * 255;

  dstMaskShow.copyTo(outputImg(Range(dstMaskStartY, dstMaskEndY),
                               Range(dstMaskStartX, dstMaskEndX)));
}

void testGrowImg() {
  Mat imgSrc = cv::imread("./imgs/img2.png", cv::IMREAD_GRAYSCALE);
  Mat imgDst = imgSrc.clone();
  Mat dstMask = cv::Mat::ones(imgDst.size(), CV_8UC1);
  imgDst(cv::Range(50, 90), cv::Range(50, 90)) = 0;
  dstMask(cv::Range(50, 90), cv::Range(50, 90)) = 0;

  const int windowR = 11;
  growImg(imgSrc, windowR, imgDst, dstMask);
  Mat imgOutput;
  stichImg(imgSrc, imgDst, dstMask, imgOutput);

  cv::imshow("stitch img", imgOutput);
  cv::waitKey(0);

  cv::imwrite("./imgs/imgOutput.jpg", imgOutput);
}

void testGrowImg2() {
  Mat imgSrc = cv::imread("./imgs/img2.png", cv::IMREAD_GRAYSCALE);
  int h = imgSrc.rows;
  int w = imgSrc.cols;

  Mat imgDst = Mat::zeros(h * 4, w * 4, CV_8UC1);
  imgSrc(Range(imgSrc.rows / 2 - 1, imgSrc.rows / 2 + 2),
         Range(imgSrc.cols / 2 - 1, imgSrc.cols / 2 + 2))
      .copyTo(imgDst(Range(imgDst.rows / 2 - 1, imgDst.rows / 2 + 2),
                     Range(imgDst.cols / 2 - 1, imgDst.cols / 2 + 2)));
  Mat dstMask = Mat::zeros(imgDst.size(), CV_8UC1);
  dstMask(Range(imgDst.rows / 2 - 1, imgDst.rows / 2 + 2),
          Range(imgDst.cols / 2 - 1, imgDst.cols / 2 + 2)) = 1;

  const int windowR = 11;
  growImg(imgSrc, windowR, imgDst, dstMask);
  Mat imgOutput;
  stichImg(imgSrc, imgDst, dstMask, imgOutput);

  cv::imshow("stitch img", imgOutput);
  cv::waitKey(0);

  cv::imwrite("./imgs/imgOutput.jpg", imgOutput);
}

void testGetUnfilledNeighbors() {
  Mat mask(51, 51, CV_8UC1);
  mask(Range(11, 31), Range(11, 31)) = 1;

  list<Pixel> pixelList;
  getUnfilledNeighbors(mask, pixelList);

  //   for (Pixel pixel : pixelList) {
  //       std::cout << pixel.y << " " << pixel.x << " " << pixel.count <<
  //       std::endl;
  //   }

  Pixel front = pixelList.front();
  Pixel back = pixelList.back();

  if ((front.count != 1) || (back.count != 3)) {
    std::cout << __FUNCTION__ << ": [FAIL]" << std::endl;
  } else {
    std::cout << __FUNCTION__ << ": [OK]" << std::endl;
  }
}

void testFindMatches() {
  Mat imgSrc = Mat::zeros(51, 51, CV_8UC1);
  imgSrc(Range(0, 11), Range(0, 11)) = 255;

  Mat imgTemp = Mat::ones(11, 11, CV_8UC1) * 255;
  Mat maskTemp = Mat::ones(11, 11, CV_8UC1);
  maskTemp(Range(3, 7), Range(2, 5)) = 0;

  vector<Pixel> bestMatches;

  findMatches(imgSrc, imgTemp, maskTemp, 5, bestMatches);

  bool ret = false;
  for (Pixel p : bestMatches) {
    // p.show();

    if ((p.x == 5) && (p.y == 5) && (p.error = 0)) {
      ret = true;
    }
  }

  if (ret == true) {
    std::cout << __FUNCTION__ << ": [FAIL]" << std::endl;
  } else {
    std::cout << __FUNCTION__ << ": [OK]" << std::endl;
  }
}
