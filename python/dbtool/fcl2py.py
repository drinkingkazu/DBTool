import json

def Parse( s ):
    """Interprets  as a dictionary"""

    Tokens = [ ':', '{', '}', ',', '[', ']', ]

    # add spaces around the tokens
    t = s
    for token in Tokens:
        t = t.replace(token, " " + token + " ")

    # split into words
    w = t.split()
    # add commas
    tw = []
    word_holder = 'NONE'
    append_comma = False
    for iWord, word in enumerate(w):
        if word not in Tokens and word[0] != '"':
            if word_holder == 'NONE':
                tw.append('"'+word+'"')
                append_comma = True
            elif word[-1] == '"':
                word = '%s %s' % ( word_holder, word )
                tw.append('"\\\"%s\\\""' % word.replace('"',''))
                word_holder = 'NONE'
                append_comma = True
            else:
                word_holder = '%s %s' % ( word_holder, word )
                append_comma = False
        elif word[0] == '"':
            if word[-1] == '"':
                #print '"\"%s\""' % word.replace('"','')
                tw.append('"\\\"%s\\\""' % word.replace('"',''))
                append_comma = True
            else:
                word_holder = word
                append_comma = False
            
        else:
            tw.append(word)
            append_comma = True
        if word not in [ ':', '{', '[', ',' ]:
            if iWord+1 < len(w) and not w[iWord+1] in [ ':', '}', ']', ',' ] and append_comma:
                tw.append(",")

    js = " ".join(tw)
    js = "{ " + js + " }"
    #print js
    d = json.loads(js)
    #print d
    return d
# Parse()
