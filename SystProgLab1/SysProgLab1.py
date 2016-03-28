import re

def count_vowels(string)-> int:
    """ This function counts number of vowels in string and returns this number"""
    set_of_vowels = {'a', 'e', 'i', 'o', 'u', 'y','а','е','ё','и','о','у','ы','э','ю','я','і','ї'}
    num_of_vowels = 0
    for char in string:
        if char.lower() in set_of_vowels:
            num_of_vowels += 1
    return num_of_vowels

set_of_words = set()#we are using set because set doesn't
                    #allow the same elements

for line in open(r'Input.txt','r'):
    set_of_words.update(re.findall('([a-zа-яёії]+)',line,re.IGNORECASE))

dictionary = {}
for word in set_of_words:
    if len(word) <= 30:
        dictionary[word] = count_vowels(word)/len(word)
    else:
        word = word[0:30]
        dictionary[word] = count_vowels(word)/len(word)
result = sorted(dictionary, key = dictionary.__getitem__)
output = open(r'OutputLab1.txt','w')
output.write('\n'.join([str(i + 1) + '. ' +
                        result[i] + ' (' + 
                        str(dictionary[result[i]]) + ')'
                        for i in range(len(result))]))
output.flush()
