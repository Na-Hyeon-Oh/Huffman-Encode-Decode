#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 26
#define MAX_TREE_Height 50

typedef struct InputCharacters {
    char alphabet;
    int freq;
}INPUT;

//HUFFMAN TREE Node
typedef struct MaxHeapNode {
    char alphabet;
    int freq;
    struct MaxHeapNode* left;
    struct MaxHeapNode* right;
}NODE;

typedef struct MaxHeapNode2 {
    char alphabet;
    struct MaxHeapNode2* left;
    struct MaxHeapNode2* right;
}NODE2;

//Heap
typedef struct Heap {
    int size;
    int capacity;
    NODE** arr;//array of max-heap node ptrs 
}Heap;

void sort(INPUT data[], int size);
NODE* newNode(char alphabet, int freq);
void maxHeapify(Heap* maxHeap, int index);
NODE* getMax(Heap* maxHeap);
void insertHeap(Heap* maxHeap, NODE* node);
void swapNode(NODE** a, NODE** b);
void buildMaxHeap(Heap* maxHeap);
Heap* MaxHeap(INPUT data[], int size);
NODE* buildHuffmanTree(INPUT data[], int size);
void preorder_traversal(NODE* ptr, FILE* fp);
void encode_toBinaryCode(NODE* root, int arr[], int top, char** encoded);
NODE2* newNode2(char alphabet);
void relocateNode(NODE2* curN, NODE2* root, int signal);

int main(void)
{
    FILE* fp_i = NULL;//file pointer for input file
    FILE* fp_o1 = NULL;//file pointer for output file1
    FILE* fp_o2 = NULL;//file pointer for output file2

    char error_message[] = "NO INPUT\n";

    //file open
    fopen_s(&fp_i, "hw3_input_.txt", "rt");
    if (fp_i == NULL)
    {
        perror(error_message);
        return 0;
    }//Exception :: No Input file

    /***********************************************************************************************************************/
    // Read hw3_input_.txt
    // input file을 두 번 읽음

    int i = 0;
    char buf;
    int freq_alphabet[MAX] = { 0, };
    int index = 0;
    int input_length = 0;//store length of input string
    char* input = '\0';

    fread(&buf, 1, 1, fp_i);

    while (!feof(fp_i))
    {
        //convert to lower case
        if (buf > 64 && buf < 91) buf = tolower(buf);//upper case
        else if (buf > 96 && buf < 123); //lower case
        //ignore other characters except for alphabet
        else
        {
            fread(&buf, 1, 1, fp_i);
            continue;
        }

        input_length++;

        //calculate frequency of each alphabet character
        index = buf - 97;//a = 97
        freq_alphabet[index]++;

        fread(&buf, 1, 1, fp_i);
    }

    //file close
    fclose(fp_i);
    fopen_s(&fp_i, "hw3_input_.txt", "rt");//re-open input file to get the input string

    //get input string
    input = (char*)malloc(input_length + 1);
    input[input_length] = '\0';
    i = 0;
    fread(&buf, 1, 1, fp_i);
    while (!feof(fp_i))
    {
        if (buf > 64 && buf < 91) buf = tolower(buf);
        else if (buf > 96 && buf < 123);
        else
        {
            fread(&buf, 1, 1, fp_i);
            continue;
        }

        input[i++] = buf;

        fread(&buf, 1, 1, fp_i);
    }


    //***********************************************************************************************************************
    // Compression to hw3_output1.txt

    fopen_s(&fp_o1, "hw3_output1.txt", "wt");

    char delimeter[] = "HEADEREND\0";
    int size = 0;
    NODE* root;
    int arr[MAX_TREE_Height];
    int top = 0;
    INPUT input_data[MAX];
    char encoded_root = '0';
    char** encoded_alphabet;
    int tmp_index;
    int tmp_length = 0;

    //Make Huffman tree
    for (i = 0; i < MAX; i++)
    {
        if (freq_alphabet[i] != 0)
        {
            input_data[size].alphabet = i + 97;
            input_data[size].freq = freq_alphabet[i];
            size++;
        }
    }

    if (size <= 1)
    {
        //Get header
        root = buildHuffmanTree(input_data, size);
        preorder_traversal(root, fp_o1);

        //write delimeter
        fwrite(delimeter, 9, 1, fp_o1);

        //Append binary encoding codes of input string
        fwrite(&encoded_root, 1, 1, fp_o1);
    }
    else
    {
        //Sort the input_data according to the frequency
        sort(input_data, size);

        //Build huffman tree and encode each character to binary code
        root = buildHuffmanTree(input_data, size);

        encoded_alphabet = (char**)malloc(sizeof(char*) * MAX);
        for (i = 0; i < MAX; i++)
        {
            encoded_alphabet[i] = (char*)malloc(MAX_TREE_Height);
            encoded_alphabet[i][0] = '\0';
        }
        encode_toBinaryCode(root, arr, top, encoded_alphabet);

        //Get header
        preorder_traversal(root, fp_o1);

        //write delimeter
        fwrite(delimeter, 9, 1, fp_o1);

        //Append binary encoding codes of input string
        for (i = 0; i < input_length; i++)
        {
            tmp_index = input[i] - 97;
            while (encoded_alphabet[tmp_index][tmp_length] != '\0')
            {
                tmp_length++;
            }
            fwrite(encoded_alphabet[tmp_index], tmp_length, 1, fp_o1);
            tmp_length = 0;
        }

        //free
        for (i = 0; i < MAX; i++)
        {
            free(encoded_alphabet[i]);
        }
        free(encoded_alphabet);
    }

    free(input);

    fclose(fp_o1);

    /***********************************************************************************************************************/
    // Read hw3_output1.txt
    // Uncompression to hw3_output2.txt

    char prev_char = NULL;
    char cur_char = NULL;
    NODE2* curNode = newNode2(NULL);
    NODE2* root2 = newNode2(NULL);
    char tmp_char;
    char separator = ':';
    char enter = '\n';
    char curBinary[MAX];

    //open output1 file to uncompress
    fopen_s(&fp_o1, "hw3_output1.txt", "rt");
    fopen_s(&fp_o2, "hw3_output2.txt", "wt");
    if (fp_o1 == NULL)
    {
        perror(error_message);
        return 0;
    }

    //Read header part(until the  delimeter)
    i = 0;
    fread(&buf, 1, 1, fp_o1);
    while (!feof(fp_o1))
    {
        if (buf == delimeter[i])
        {
            i++;
            if (i == 9) break;
        }
        else
        {
            i = 0;

            cur_char = buf;
            if (cur_char == '+')
            {
                //add node(+) to left child
                if (prev_char == '+')
                {
                    curNode->left = newNode2(cur_char);
                    relocateNode(curNode, root2, 0);
                }

                else if (prev_char == NULL)
                {
                    root2->alphabet = cur_char;
                    curNode = root2;
                }

                //prev_char is alphabet
                //add node(+) to right child
                else
                {
                    curNode->right = newNode2(cur_char);
                    relocateNode(curNode, root2, 1);
                }
            }
            else//cur_char is alphabet(lower-case)
            {
                //add node(alphabet) to left child
                if (prev_char == '+')
                {
                    curNode->left = newNode2(cur_char);
                    //relocateNode(curNode, root2, 0);
                }

                else if (prev_char == NULL)
                {
                    root2->alphabet = cur_char;
                    curNode = root2;
                }

                //prev_char is alphabet
                //add node(alphabet) to right child
                else
                {
                    curNode->right = newNode2(prev_char);
                    relocateNode(curNode, root2, 2);
                }
            }

            prev_char = cur_char;
        }

        fread(&buf, 1, 1, fp_o1);
    }


    encoded_alphabet = (char**)malloc(sizeof(char*) * MAX);
    for (i = 0; i < MAX; i++)
    {
        encoded_alphabet[i] = (char*)malloc(MAX_TREE_Height);
        encoded_alphabet[i][0] = '\0';
    }
    encode_toBinaryCode(root, arr, top, encoded_alphabet);


    //Print sorted binary encoding results
    for (i = 0; i < MAX; i++)
    {
        if (encoded_alphabet[i][0] == '\0') continue;

        tmp_char = 97 + i;
        tmp_length = 0;
        while (encoded_alphabet[i][tmp_length] != '\0')
        {
            tmp_length++;
        }
        fwrite(&tmp_char, 1, 1, fp_o2);
        fwrite(&separator, 1, 1, fp_o2);
        fwrite(encoded_alphabet[i], tmp_length, 1, fp_o2);
        fwrite(&enter, 1, 1, fp_o2);
    }

    //Read encoded part to decode
    i = 0;
    fread(&buf, 1, 1, fp_o1);
    while (!feof(fp_o1))
    {
        curBinary[i++] = buf;
        curBinary[i] = '\0';
        tmp_length = i;
        i = 0;
        while (i < MAX)
        {
            if (strcmp(encoded_alphabet[i], curBinary) != 0) i++;

            //If leaf node, write corresponding character to file
            else
            {
                tmp_char = 97 + i;
                fwrite(&tmp_char, 1, 1, fp_o2);

                for (i = 0; i < MAX; i++) curBinary[i] = '\0';
                tmp_length = 0;
                break;
            }
        }
        i = tmp_length;

        fread(&buf, 1, 1, fp_o1);
    }


    for (i = 0; i < MAX; i++)
    {
        free(encoded_alphabet[i]);
    }
    free(encoded_alphabet);

    fclose(fp_o1);
    fclose(fp_o2);


    //***********************************************************************************************************************


    return 0;
}


// Insertion Sort
//since there are at most 26 elements
void sort(INPUT data[], int size)
{
    int i, j;
    int key_freq, key_alpa;

    for (i = 1; i < size; i++)
    {
        key_freq = data[i].freq;
        key_alpa = data[i].alphabet;
        j = i - 1;

        while (j >= 0 && data[j].freq < key_freq)
        {
            data[j + 1] = data[j];
            j = j - 1;
        }
        data[j + 1].freq = key_freq;
        data[j + 1].alphabet = key_alpa;
    }
}

// Make new-node for compressed Huffman Tree
NODE* newNode(char alphabet, int freq)
{
    NODE* tmp = (NODE*)malloc(sizeof(NODE));
    tmp->alphabet = alphabet;
    tmp->freq = freq;
    tmp->left = NULL;
    tmp->right = NULL;
    return tmp;
}

// Heapify
void maxHeapify(Heap* maxHeap, int index)
{
    int max = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if ((left < maxHeap->size) && (maxHeap->arr[left]->freq < maxHeap->arr[max]->freq)) max = left;
    if ((right < maxHeap->size) && (maxHeap->arr[right]->freq < maxHeap->arr[max]->freq)) max = right;

    if (index != max)
    {
        swapNode(&maxHeap->arr[index], &maxHeap->arr[max]);
        maxHeapify(maxHeap, max);
    }
}

// Extract the node having maximum value on the top
NODE* getMax(Heap* maxHeap)
{
    NODE* tmp = maxHeap->arr[0];
    maxHeap->arr[0] = maxHeap->arr[maxHeap->size - 1];
    --maxHeap->size;
    maxHeapify(maxHeap, 0);
    return tmp;
}

// Insert new node
void insertHeap(Heap* maxHeap, NODE* node)
{
    int presize = maxHeap->size;
    ++maxHeap->size;

    while (presize && (node->freq < maxHeap->arr[(presize - 1) / 2]->freq))
    {
        maxHeap->arr[presize] = maxHeap->arr[(presize - 1) / 2];
        presize = (presize - 1) / 2;
    }

    maxHeap->arr[presize] = node;
}

// Swap two nodes of max heap
void swapNode(NODE** a, NODE** b)
{
    NODE* tmp = *a;
    *a = *b;
    *b = tmp;
}

// Build the max heap 
void buildMaxHeap(Heap* maxHeap)
{
    int i;
    int n = maxHeap->size - 1;
    for (i = (n - 1) / 2; i >= 0; --i) maxHeapify(maxHeap, i);
}

// Create and Build a max heap
Heap* MaxHeap(INPUT data[], int size)
{
    int i;

    //Create a new heap
    Heap* maxHeap = (Heap*)malloc(sizeof(Heap));
    maxHeap->size = 0;
    maxHeap->capacity = size;
    maxHeap->arr = (NODE**)malloc(sizeof(NODE*) * maxHeap->capacity);

    //make nodes
    for (i = 0; i < size; i++) maxHeap->arr[i] = newNode(data[i].alphabet, data[i].freq);
    maxHeap->size = size;

    buildMaxHeap(maxHeap);

    return maxHeap;
}

// Build Huffman tree 
NODE* buildHuffmanTree(INPUT data[], int size)
{
    NODE* left, * right;
    NODE* top;

    //Create a max heap
    Heap* maxHeap = MaxHeap(data, size);

    while (!(maxHeap->size == 1))//while size is not 1
    {
        //Extract the two nodes having low frequency
        left = getMax(maxHeap);
        right = getMax(maxHeap);

        //Create a new internal node and add to the heap 
        top = newNode('+', left->freq + right->freq);// '+' is a special value for internal nodes, not used 
        top->left = left;
        top->right = right;

        insertHeap(maxHeap, top);
    }

    // The remaining node is the root node and the tree is complete
    return getMax(maxHeap);
}

//Traverse Huffman Tree in post-order
void preorder_traversal(NODE* ptr, FILE* fp)
{
    int print_char;

    if (ptr) {
        //printf("%c : %d", ptr->alphabet, ptr->freq);
        print_char = ptr->alphabet;
        fwrite(&print_char, 1, 1, fp);
        preorder_traversal(ptr->left, fp);
        preorder_traversal(ptr->right, fp);
    }
}

// Make huffman code from the root of Huffman Tree
//arr[] : to store codes as integer
//->encoded[] : codes as character to print
void encode_toBinaryCode(NODE* root, int arr[], int top, char** encoded)
{
    int tmp_index = 0;

    //Assign 0 to left edge
    if (root->left)
    {
        arr[top] = 0;
        encode_toBinaryCode(root->left, arr, top + 1, encoded);
    }

    //Assign 1 to right edge
    if (root->right)
    {
        arr[top] = 1;
        encode_toBinaryCode(root->right, arr, top + 1, encoded);
    }

    //If this is a leaf node, then store the binary code from arr[] 
    if (!(root->left || root->right))
    {
        int i;
        char tmp_encoded[MAX_TREE_Height] = { '\0', };

        tmp_index = root->alphabet - 97;
        for (i = 0; i < top; ++i)
        {
            tmp_encoded[i] = arr[i] + '0';
        }
        tmp_encoded[i] = '\0';

        strcpy_s(encoded[tmp_index], sizeof(tmp_encoded), tmp_encoded);
    }
}

// Make new node for uncompressed huffman tree
NODE2* newNode2(char alphabet)
{
    NODE2* tmp = (NODE2*)malloc(sizeof(NODE2));
    tmp->alphabet = alphabet;
    tmp->left = NULL;
    tmp->right = NULL;
    return tmp;
}

void relocateNode(NODE2* curN, NODE2* root, int signal)
{
    //to the left child
    if (signal == 0) curN = curN->left;

    //to the right child
    else if (signal == 1) curN = curN->right;

    //to the front node
    //until there is the node which has left child but no right child
    else
    {
        curN = root;
        while (curN->right != NULL)
        {
            curN = curN->right;
        }
    }
}