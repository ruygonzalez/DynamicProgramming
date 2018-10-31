//
//  align.cpp
//  dna_alignment
//
//  Created by Erika DeBenedictis on 1/27/13.
//  Copyright (c) 2014 California Institute of Technology. All rights reserved.
//
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

// scoring values
#define GAP_SCORE -5
#define MISMATCH -1
#define MATCHING 2

/**
 * @brief Packages the score and instruction string the align function returns
 */
struct align_result {
    int score;      // score of this alignment
    string inst;    // instruction on how to align the inputs

    align_result(int s, string i) {
        this->score = s;
        this->inst = i;
    }
    align_result() {
        this->score = 0;
        this->inst = "";
    }
};

// memo_type will allow us to hash the string input to align
// with its output for memoization
typedef unordered_map<string, align_result> memo_type;

/** 
 * Helper function, returns align_result with greater score
 */
align_result getmax(align_result a, align_result b){
	if(a.score > b.score)
		return a;
	return b;
}

/**
 * @brief Function takes two strings, s and t, and produces an align_result
 * of the highest alignment score and its corresponding instruction str.
 */
align_result align(string s, string t, memo_type &memo) {
    // if this result is memoized, use recorded result
    string key = s + "," + t;
    if (memo.count(key) > 0){
      return memo[key];
    }
	int slen = s.size();
	int tlen = t.size();
	align_result answer;
	if (slen == 0){
		answer.score = (tlen - slen) * GAP_SCORE;
		answer.inst = "";
		for(int i = 0; i < (tlen - slen); i++)
			answer.inst += "t";
		memo[key] = answer;
		return answer;
    }
	if (tlen == 0){
		answer.score = (slen - tlen) * GAP_SCORE;
		answer.inst = "";
		for(int i = 0; i < (slen - tlen); i++)
			answer.inst += "s";
		memo[key] = answer;
		return answer;
    }
	string s1 = s.substr(0,1);
	string t1 = t.substr(0,1);
	if (s1 == t1 && s1 != "-"){
		answer.score = MATCHING;
		answer.inst = "|";
	}
	else if (s1 == "-" || s1 == " "){
		answer.score = GAP_SCORE;
		answer.inst = "t";
    }
	else if (t1 == "-" || t1 == " "){
		answer.score = GAP_SCORE;
		answer.inst = "s";
    }
	else{
		answer.score = MISMATCH;
		answer.inst = "*";
	}
	if (slen > 1)
		s = s.substr(1);
	else
		s = "";
	if (tlen > 1)
		t = t.substr(1);
	else
		t = "";
	string sa = "-" + s;
	string ta = "-" + t;
	align_result temp1 = getmax(align(sa, t, memo), align(s, ta, memo));
	align_result temp = getmax(temp1, align(s, t, memo));
	answer.score += temp.score;
	answer.inst += temp.inst;
	memo[key] = answer;
	return answer;
}


/**
 * @brief Wrapper function to print the results of align
 */
void DNA_align(string s, string t) {
    cout << endl << "Calling DNA align on strings " << s << ", " << t << endl;

    // create the memoization system
    memo_type memo;
	string sa = "-" + s;
	string ta = "-" + t;
	align_result temp1 = getmax(align(sa, t, memo), align(s, ta, memo));
	align_result answer = getmax(temp1, align(s, t, memo));
    string ans = answer.inst;
    int score = answer.score;

    // Printing section
    // line where string s will be printed, spaces inserted
    string line1 = "";
    // line where string t will be printed, spaces inserted
    string line2 = "";
    // description of the relationship between s and t here (* | s t)
    string line3 = "";

    int j = 0;      // running index in s
    int k = 0;      // running index in t

    for (unsigned int m = 0; m < ans.length(); m++) {
        // i is the next element in our instruction string ans
        string i = ans.substr(m, 1);

        // only in s
        if(i.compare("s") == 0){
            line1 += s[j]; j++;
            line2 += " ";
            line3 += "s";
        }

        // only in t
        else if (i.compare("t") == 0){
            line1 += " ";
            line2 += t[k]; k++;
            line3 += "t";
        }

        // mismatch
        else if (i.compare("*") == 0){
            line1 += s[j]; j++;
            line2 += t[k]; k++;
            line3 += "*";
        }

        // match
        else {
            line1 += s[j]; j++;
            line2 += t[k]; k++;
            line3 += "|";
        }
    }
    cout << line1 << endl << line2 << endl << line3 << endl;
    cout << "Score for this alignment: " << score << endl;
}

int main(){
    // some test cases to begin with
    DNA_align("",   "a");
	cout << "Supposed to be 't' and score: -5" << endl;
	DNA_align("a", "");
	cout << "Supposed to be 's' and score: -5" << endl;
	DNA_align("a","b");
	cout << "Supposed to be '*' and score: -1" << endl;
    //DNA_align("b",  "");
    DNA_align("a", "a");
	cout << "Supposed to be '|' and score: 2" << endl;
    //DNA_align("b",  "a");
    DNA_align("b",  "ba");
    cout << "Supposed to be '|t' and score: -3" << endl;
	DNA_align("b", "ab");
	cout << "Supposed to be 't|' and score: -3" << endl;
    DNA_align("ab", "ba");
	cout << "Supposed to be '**' and score: -2" << endl;
    DNA_align("ab", "b");
	cout << "Supposed to be 's|' and score: -3" << endl;
	DNA_align("ba", "b");
	cout << "Supposed to be '|s' and score : -3" << endl;
	DNA_align("abc", "ac");
	cout << "Supposed to be '|s|' and score: -1" << endl;
	DNA_align("abc", "adc");
	cout << "Supposed to be '|*|' and score: 3" << endl;
	DNA_align("abracadabra", "avada kedavra");
	cout << "Supposed to be '|**t|**t||*||' and score: -3" << endl;
	DNA_align("ACTGGCCGT", "TGACGTAA");
	DNA_align("gttaacggat tgtgttatcc cgggaatgta gaaaatctag aagaattaag gtcacttttt agttctgctc 	ggtcttatca gaggatcctg attttcccag acacaatctg gaatgtgtct tacagtggga caagcaaagc atgttcagat tcattctaca gaagcatgag atggttgact caaaagaaca acgcttaccc tattcaagac gcccaatac", "gttaacggat tgtgttatcc cgggactgta gaaaatctag aagaattaag gtcacttttt agttctgctc ggtcttatca gaggatcctg attttcccag acacaatctg gaatgtgtct tacagtggga caagcaaagc atgttcggat tcattctaca gaagcatgag atggttgact caaaggaaca atgcttaccc tattcaagac gcccaatac");
    return 0;
}
