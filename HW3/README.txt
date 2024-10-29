i. Completed Parts:
    h1.cc h2.cc h3.cc h4.cc

ii. Bugs and Errors:
    For h1.cc, some edges within the output image appear to be a lot whiter than what
    was shown in the example expected output image.

    For h2.cc, the edges might appear fuzzier than what was shown in the expected.

    For h3.cc, the output Hough Transform image is a lot darker/grayer than the expected
    output image (which was whiter than my output).

    For h4.cc, there seems to be a lot more lines drawn in the output image, depending on
    the user-input threshold.

iii. How to run program:
    To compile each program:
        h1.cc:
        g++ h1.cc image.cc -o h1

        h2.cc:
        g++ h2.cc image.cc -o h2

        h3.cc:
        g++ h3.cc image.cc -o h3

        h4.cc:
        g++ h4.cc image.cc -o h4

    
    For running programs:
        h1.cc:
        ./h1 <input gray-level image> <output gray-level edge image>
        Ex: ./h1 hough_simple_1.pgm output_gray_edge.pgm

        h2.cc (THRESHOLD USED WAS 50):
        ./h2 <input gray-level EDGE image> <threshold> <output binary edge image>
        Ex: ./h2 output_gray_edge.pgm 50 output_binary.pgm

        h3.cc:
        ./h3 <input binary edge image> <output gray-level Hough image> <output Hough-voting array txt file>
        Ex: ./h3 output_binary.pgm output_gray_hough.pgm output_array.txt

        h4.cc (THRESHOLD USED WAS 290):
        ./h4 <input original gray-level image> <input Hough-voting array txt file> <threshold> <output gray-level line image>
        Ex: ./h4 hough_simple_1.pgm output_array.txt 290 output_grayline.pgm

iv. Input and Output Files:
    image.h
    image.cc
    thresholds.txt (50 for h2.cc, 290 for h4.cc)
    hough_simple_1.pgm (used as input in h1.cc and h4.cc)
    h1.cc (Outputted output_gray_edge.pgm)
    h2.cc (Used output_gray_edge.pgm as input) (Outputted output_binary.pgm)
    h3.cc (Used output_binary.pgm as input) (Outputted output_gray_hough.pgm and output_array.txt)
    h4.cc (used hough_simple_1.pgm and output_array.txt as input) (Outputted output_grayline.pgm)

