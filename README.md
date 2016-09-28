# Image Template Matching using OpenCV

Template matching is a technique for finding areas of an image that match (are similar) to a template image (patch).

To compile:

`g++ match_template.cpp -o match_template $(pkg-config --cflags --libs opencv)`

To test:

`./match_template lena.png lena_crop.png`

(should produce `-81 -61` because `lena_crop.png` needs to be shifted by 81 pixel down and 61 pixel to the right to match with `lena.png`)
