#include <stdio.h>
#include <stdlib.h>

#include "ptree.h"


int main(int argc, char *argv[]) {
    // Creates a ptree to test printing
    // Notice that in this tree the names are string literals. This is fine for
    // testing but is not what the assignment asks you to do in generate_ptree.
    // Read the handout carefully. 
    struct TreeNode root, child_one, child_two, child_three, child_four, grandchild1, grandchild2, grandchild3, grandchild4;
    root.pid = 4511;
    root.name = "root process";
    root.child = &child_one;
    root.sibling = NULL;

    child_one.pid = 4523;
    child_one.name = "first child";
    child_one.child = &grandchild4;
    child_one.sibling = &child_two;

    child_two.pid = 4524; 
    child_two.name = "second child";
    child_two.child = &grandchild1;
    child_two.sibling = &child_four;

    child_three.pid = 4525; 
    child_three.name = "third child";
    child_three.child = NULL;
    child_three.sibling = NULL;

    child_four.pid = 4526; 
    child_four.name = "fourth child";
    child_four.child = NULL;
    child_four.sibling = &child_three;

    grandchild1.pid = 4609;
    grandchild1.name = "grandchild1";
    grandchild1.child = NULL;
    grandchild1.sibling = NULL;
    
    grandchild2.pid = 4610;
    grandchild2.name = "grandchild2";
    grandchild2.child = NULL;
    grandchild2.sibling = NULL;
    
    grandchild3.pid = 4611;
    grandchild3.name = "grandchild3";
    grandchild3.child = NULL;
    grandchild3.sibling = NULL;
    
    grandchild4.pid = 4612;
    grandchild4.name = "grandchild4";
    grandchild4.child = &grandchild2;
    grandchild4.sibling = &grandchild3;

    print_ptree(&root, 10);

    return 0;
}

