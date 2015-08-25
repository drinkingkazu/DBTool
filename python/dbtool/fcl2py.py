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
    for iWord, word in enumerate(w):
        if word not in Tokens and word[0] != '"':
            tw.append('"'+word+'"')
        elif word[0] == '"':
            #print '"\"%s\""' % word.replace('"','')
            tw.append('"\\\"%s\\\""' % word.replace('"',''))
        else:
            tw.append(word)
        if word not in [ ':', '{', '[', ',' ]:
            if iWord+1 < len(w) and not w[iWord+1] in [ ':', '}', ']', ',' ]:
                tw.append(",")

    js = " ".join(tw)
    js = "{ " + js + " }"
    d = json.loads(js)
    return d
# Parse()
