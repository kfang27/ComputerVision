i. Completed Parts:
    s1.cc s2.cc (Partially s3.cc)

ii. Bugs and Errors:
    For s3.cc, I was unable to output a proper/the expected albedo image.
    The results were either a black image or an image that looked like the
    output normals image (but without the needles) as it just had black dots instead.
    Also, the normals output image does not have every needle drawn out as some dots
    are just itself alone while others have the needles drawn out.

iii. How to run program:
    To compile everything:
        Using the Makefile, just run "make all"

    For running programs:
        s1.cc (THRESHOLD USED WAS 100):
        ./s1 <input gray-level sphere image> <threshold value> <output parameters file>
        Ex: ./s1 sphere0.pgm 100 parameters.txt

        s2.cc:
        ./s2 <input parameters filename> <input sphere image 1 filename> <input sphere image 2 filename> <input sphere image 3 filename> <output directions filename>
        Ex: ./s2 parameters.txt sphere1.pgm sphere2.pgm sphere3.pgm directions.txt

        s3.cc (THRESHOLD USED WAS 80):
        Ex: ./s3 directions.txt object1.pgm object2.pgm object3.pgm 10 80 normals_output.pgm albedo_output.pgm


iv. Input and Output Files:
    image.h
    image.cc
    thresholds.txt (80 for s3.cc)
    sphere0.pgm (used as input for s1.cc)
    sphere1.pgm, sphere2.pgm, sphere3.pgm (used as input for s2.cc)
    object1.pgm, object2.pgm, object3.pgm (used as input for s3.cc)
    s1.cc (used 100 as threshold value, Outputted parameters.txt)
    s2.cc (Outputted directions.txt)
    s3.cc (Outputted normals_output.pgm, albedo_output.txt)

