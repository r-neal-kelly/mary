#pragma once

#include <Mary/String.h>

enum
{
  MARY_LANG_ALPHABET_ANY,
  MARY_LANG_ALPHABET_LATIN,
  MARY_LANG_ALPHABET_HEBREW,
  MARY_LANG_ALPHABET_GREEK
};

// regex could be really useful here.

uint64_t Mary_Lang_Alphabet(); // tells what alphabet the letters belong.
void Mary_Lang_Is_Latin(); // tests if all letters are in the latin alphabet
void Mary_Lang_Is_Hebrew();
void Mary_Lang_Is_Greek();
void Mary_Lang_Cluster(); // groups not in bytes, not in units, not in codes, but as many codes as fit in a cluster
