i. Completed Parts:
    p1.cc p2.cc p3.cc

ii. Bugs and Errors:
    For p2.cc, a different gray-level doesn't seem to be applied properly when
    encountering a new object. Sometimes, the second object ends up being blacked out,
    thus resulting in it blending in with the black background.

    For p3.cc, the linear segment or line doesn't seem to be drawn properly or at all.
    Also, the objects are no longer shown in the image (assuming that they shouldn't be excluded
    or touched at all). 

iii. How to run program:
    To compile everything:
        For the Makefile, the P2 line:
        CC_OBJ_2=image.o DisjSets.o p2.o
        was changed to
        CC_OBJ_2=image.o p2.o
        because the DisjSets files weren't used.

        Using the Makefile, just run "make all"
    
    For running programs:
        p1.cc (THRESHOLD VALUE used was 128):
            ./p1 <input_image.pgm> <threshold> <binary_image.pgm>
            Example: ./p1 two_objects.pgm 128 binary_two_objects.pgm

        p2.cc ():
            ./p2 <input_binary_image.pgm> <labeled_image.pgm>
            Example: ./p2 binary_two_objects.pgm labeled_two_objects.pgm
        
        p3.cc :
            ./p3 <input_labeled_image.pgm> <output_object_descriptions.txt> <labeled_image.pgm>
            Example: ./p3 labeled_two_objects.pgm object_descriptions.txt output_image.pgm

iv. Input and Output Files:
    image.h
    image.cc 
    two_objects.pgm (used as input in p1.cc)
    p1.cc (Outputted binary_two_objects.pgm)
    p2.cc (Used binary_two_objects.pgm as input) (Outputted labeled_two_objects.pgm)
    p3.cc (Used labeled_two_objects.pgm as input) (Outputted object_descriptions.txt and output_image.pgm)

