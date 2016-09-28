#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "polyfit.hpp"

using namespace std;
using namespace cv;


int match(String src_image, String templ_image, float &dx, float &dy, float &h) {
    float h_i, h_j, h_new;
    Mat src_img_data;
    Mat teml_img_data;
    Mat cmat;
    Mat peak;
    float i, j, i_new, j_new;
    float SIGMA = 1.5;
    int NP = 5;
    int DEGREE = 2;

    std::vector<float> col_x(NP), row_x(NP), col_y(NP), row_y(NP);
    std::vector<float> coeff_col(DEGREE + 1), coeff_row(DEGREE + 1);
    src_img_data = imread(src_image, CV_LOAD_IMAGE_GRAYSCALE);
    teml_img_data = imread(templ_image, CV_LOAD_IMAGE_GRAYSCALE);
    Mat src_img_data_filtered(src_img_data.size(), CV_32F);
    Mat teml_img_data_filtered(teml_img_data.size(), CV_32F);

    GaussianBlur(src_img_data, src_img_data_filtered, Size(0, 0), SIGMA, SIGMA);
    GaussianBlur(teml_img_data, teml_img_data_filtered, Size(0, 0), SIGMA, SIGMA);

    int cmat_cols = src_img_data.cols - teml_img_data.cols + 1;
    int cmat_rows = src_img_data.rows - teml_img_data.rows + 1;
    

    cmat.create(cmat_rows, cmat_cols, CV_32FC1);
    matchTemplate(src_img_data_filtered, teml_img_data_filtered, cmat, TM_CCORR_NORMED);

    normalize(cmat, cmat, 0, 1, NORM_MINMAX, -1);

    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;

    minMaxLoc(cmat, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    matchLoc = maxLoc;
    
    i = matchLoc.x;
    j = matchLoc.y;
    h = cmat.at<float>(j, i);

    int start_i = i - (NP - 1) / 2;
    int end_i = i + (NP + 1) / 2;
    int start_j = j - (NP - 1) / 2;
    int end_j = j + (NP + 1) / 2;

    for (int ii = start_i; ii <= end_i; ii++) {
        row_x[ii - start_i] = ii;
        row_y[ii - start_i] = cmat.at<float>(j, ii);
    }
    for (int ii = start_j; ii <= end_j; ii++) {
        col_x[ii - start_j] = ii;
        col_y[ii - start_j] = cmat.at<float>(ii, i);
    }

    coeff_row = polyfit(row_x, row_y, DEGREE);
    coeff_col = polyfit(col_x, col_y, DEGREE);
    j_new = -coeff_col[1] / (2 * coeff_col[2]);
    i_new = -coeff_row[1] / (2 * coeff_row[2]);
    h_i = coeff_col[0] + coeff_col[1] * i_new + coeff_col[2] * i_new * i_new;
    h_j = coeff_row[0] + coeff_row[1] * j_new + coeff_row[2] * j_new * j_new;
    h_new = std::max(h_i, h_j);
    if (( h_new > 0 ) && ( h_new < 1)) {
        i = i_new;
        j = j_new;
        h = h_new;
    }

    dx = i - (src_img_data.cols - teml_img_data.cols) / 2;
    dy = j - (src_img_data.rows - teml_img_data.rows) / 2;
    return 0;
}

int main(int argc, char **argv) {
    float dx, dy, h;
    String src_image = argv[1];
    String templ_image = argv[2];
    match(src_image, templ_image, dx, dy, h);
    std::cout << dx << " " << dy << std::endl;
    return 0;
}

