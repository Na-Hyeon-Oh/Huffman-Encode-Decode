# File Compression and Decompression Using Huffman Tree

✅ Algorithms_PA3

<br>

## Index
+ [Overview](#ov)
+ [Tools](#with)
+ [Prerequisites](#pre)
+ [Details](#details)

<br><br>

## Overview <a name = "ov"></a>

Implementation of a program that compresses and uncompresses files using Huffman coding.


<br>

## Tools <a name = "with"></a>

<a href="https://github.com/search?q=user%3ADenverCoder1+is%3Arepo+language%3Ac"><img alt="C" src="https://img.shields.io/badge/C-2370ED.svg?logo=c&logoColor=white"></a>

✔️ Window 10

✔️ Visual Studio


<br>

## Prerequisites <a name = "pre"></a>

❓ What is Huffman Code?

🅰️ 



<br>

## Details <a name = "details"></a>

### Description of Files

|File Name|Descripton|
|------|---|
|sort.c|main file|
|input|input files / test cases|
|output|output file|

<br>

### Compile and Execute

Your main file should be in the folder where input files are in.

<br>

### Algorithm

⚫ Overall, I used arrays* with the size MAX(=26). From these arrays’ INDEX, I got the character by adding ‘a’=97(in integer).


<br>

⚫ Compression

  - After reading input.txt file, I get input string and each alphabets’ frequency.
  - From these data, I make INPUT data {alphabet+frequency} from structure, and sort them in the descending order of frequency using Insertion sort.
  - Then I make Huffman Tree using Max Heap with NODE structure.
  - To print the header, I use the method of pre-order traversal.
  - To print the encoded binary strings, I use the predefined arrays* named ‘encoded_alphabet[]’.

<br>

⚫ Decompression

  - While reading output1.txt file, I reconstruct Huffman Tree with NODE2 structure by using the rules of the results of pre-order traversal(using current charcter and previous character).
  - Furthermore, I decode from the encoded binary strings using re-used-predefined arrays*.
  - Specifically, by comparing predefined arrays and reading binary strings, we can know which point is leaf node of the Huffman tree, and then print out the corresponding alphabet.


<br>

### Performance Analysis

Input file read twice, linearly

-	If (the length of input string) = w, then read this 1 byte for twice. Therefore, O(2w)=O(w)
-	I found that functions such as getline() has bigger internal time complexity than fread(), fwrite(). So that I use the latter functions

<br>

Compression

-	n<m

1.	sort()
      -	Let (the number of used alphabet) = n
      -	I used Insertion sort to make set of alphabets data in which contains information such as alphabet and frequency(type INPUT), because the number of alphabets is at most 26, and I think this is quite small number in computer calculation.
      -	Sort in the descending order of frequency.
      -	This data would be used for making Huffman tree.
      -	▶️ Θ(n^2)

2. Make Huffman Tree using Max Heap
      -	Let (the number of node) =m. Then extractMax() is called 2*(m-1) times which takes O(log m) time. extractMax() calls maxHeapify().
      -	▶️ O(m log m)
      

<br>

Decompression

1. 	Read file and reconstruct Huffman tree
      -	Let (the length of header) = w1 and (the number of used node) = m (same as above)
      -	I read header for once linearly so that it takes O(w1)
      -	While reading header, I could construct Huffman tree simultaneously. Because there are some function call(ex) relocate_node()) to make tree linearly, it takes some internal function call time.

2. Read remained encoded part and Decode
      -	Let (the length of encoded part) = w2 and (the number of used node) = m (same as above)
      -	It takes O(w2+c) to read hw3_output1.txt file where c is the length of delimeter
      -	While decoding, I search for the leaf nodes to print each encoded binary strings.  The way how I find leaf node is to iteratively match each index of the stored encoded strings(=encoded_alphabet[]) which can be set from the reconstructed-Huffman Tree, with the string(=curBinary) which I’ m reading from the input file.   ▶️ O(m MAX^2) where MAX=26=(the number of lower-case-alphabets)



<br>



### Input Format<a name="inputF"></a>

  
  ```
huffman

  ```

<br>


### Rules

To compress a file, your program will follow the following steps:


  - Read in the input file named ‘hw3 input.txt’, and calculate the frequencies of all alphabetic (a-z only) characters in the ASCII text file. Please ignore other characters except for alphabetic characters. Also, convert all uppercase characters into lowercase characters.

  - Construct a Huffman tree for all alphabetic (a-z only) characters that appear in the input file.

  - Output the header describing the tree to the output file named ‘hw3 output1.txt’. You can use any description to specify a binary tree.

  - Rewind the file to the beginning re-reading each symbol (character).

  - Append the (binary) encoding for each symbol to the output file ‘hw3 output1.txt’. Mark “HEADEREND” as the delimiter to separate the header from the compressed
part.


To uncompress the compressed file hw3 output1.txt, your program will follow the following steps:

  - Read in the Huffman tree from the compressed file hw3 output1.txt.

  - Use the header to reconstruct the Huffman tree.

  - Decode the input using the Huffman tree.

  - Write the binary code of each alphabetic character used in the input file and the decoded alphabetic (a-z only) characters to the output file named ‘hw3 output2.txt’. The binary codes should be sorted alphabetically.

<br>
  
### Output Format
 
1. hw3_output1. txt

  ```
((( a , m ) ,f ) ,((h , u ) ,n ) ) HEADEREND100101010100100011

  ```

 
2. hw3_output2. txt

  ```
a :000
m :001
f :01
h :100
u :101
n :11
huffman

  ```
 
