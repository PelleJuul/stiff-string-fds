import sys

if (len(sys.argv) < 2):
    print('Usage: python literalize.py file')

with open(sys.argv[1], 'r') as file:
    # The state variable describes which kind of block we are currently hand-
    # ling. Some example values are:
    #
    #   'code'      we are currently reading a code block.
    #   'comment'   -- a comment block.
    #   'blank'     -- blank lines.
    #   'function'  -- a function decleration.
    state = ''

    equationText = ''
    lastWasBlankLine = False

    for line in file.readlines():
        # This variable will hold the state we're gonna be in after handling
        # the line.
        newState = state

        s = line.strip()

        # This variable holds the text of the line that will eventually 
        # be written to the output.
        out = None;

        if s.startswith('/// $$'):
            if state == 'equation':
                newState = 'not-equation'
            else:
                newState = 'equation'
                equationText = ''

        # Comment blocks starts with a '/// ' and are converted to normal text.
        elif s.startswith('/// '):
            if state == 'equation':
                equationText = equationText + s[4:] + '\n'
            else:
                newState = 'comment'
                out = s[4:]

        # We dont want to include pragma once in the output.
        elif s.startswith("#pragma once"):
            pass

        # Blank lines are written directly to the output.
        elif s == '':
            newState = 'blank'
            out = line

        # We don't want to include class definitions, braces, and related
        # keywords.
        elif s == 'public:' or s == 'private:' or s == '{' or s == '};' or s.startswith('class'):
            newState = 'blank'

        # We assume that lines ending with ');' are function definitions. These
        # are turned into headers.
        elif s.endswith(");"):
            newState = 'function'
            out = f'#### {s[:-1]}\n\n'

        # If we did not encounter anything special we assume that were reading
        # code.
        else:
            newState = 'code'
            out = s

        # Start a markdown code block when we start reading some code.
        if newState == 'code' and state != 'code':
            print('```')

        # Stop a markdown code block when we stop reading code.
        if newState != 'code' and state == 'code':
            print('```')

        if newState != 'equation' and state == 'equation':
            print('')
            for equationLine in equationText.split('\n'):
                if (equationLine.strip() != ''):
                    print(f'> {equationLine}')
            print('')

        state = newState

        if out != None and out.strip() == "":
            if lastWasBlankLine:
                pass
            else:
                print("")
            
            lastWasBlankLine = True
        elif out != None:
            print(out)
            lastWasBlankLine = False