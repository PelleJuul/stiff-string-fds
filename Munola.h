#pragma once

#include <stack>
#include <string>
#include <algorithm>
#include <vector>

enum MunolaType
{
    Command,
    Note,
    Rest,
    End,
    Function
};

enum MunolaCommand
{
    Octave,
    Double,
    Halve,
    EndPhrase,
    Sharp,
    Flat,
};

struct Munola
{
    MunolaType type;
    MunolaCommand command;
    std::string function = "";
    bool isPhraseEnd = false;
    int octave = 4;
    int pitch = 0;
    float duration = 0.0;

    int getMidiPitch()
    {
        return 12 + 12 * octave + pitch;
    }
};

struct MunolaPlayer
{
    float fs = 44100;
    float timeUntilNextEvent = 0;
    float tempo = 60;
    int i = 0;
    bool loop = false;
    std::vector<Munola> stack;

    MunolaPlayer(std::vector<Munola> &source, float fs = 44100) : stack(source)
    {
        this->fs = fs;
    }

    bool newEventAvailable()
    {
        if (i >= stack.size())
        {
            if (loop)
            {
                i = 0;
            }

            return false;
        }

        return timeUntilNextEvent <= 0;
    }

    void proceed()
    {
        if (timeUntilNextEvent > 0)
        {
            timeUntilNextEvent -= 1.0 / fs;
        }
    }

    Munola consumeNextEvent()
    {
        Munola m = stack[i];
        i++;
        timeUntilNextEvent = (60.0 / tempo) * m.duration;
        return m;
    }
};

void evalMunola(std::vector<Munola> &stack)
{
    if (stack.empty())
    {
        return;
    }

    Munola m = stack.back();
    stack.pop_back();

    if (m.type == MunolaType::Note)
    {
        evalMunola(stack);
        stack.push_back(m);
    }

    if (m.type == MunolaType::Command)
    {
        if (m.command == MunolaCommand::Octave)
        {
            evalMunola(stack);
            Munola b = stack.back();
            stack.pop_back();
            b.octave = m.octave;
            stack.push_back(b);
        }

        if (m.command == MunolaCommand::Double)
        {
            evalMunola(stack);
            Munola b = stack.back();
            stack.pop_back();
            b.duration *= 2;
            stack.push_back(b);
        }

        if (m.command == MunolaCommand::Halve)
        {
            evalMunola(stack);
            Munola b = stack.back();
            stack.pop_back();
            b.duration *= 0.5;
            stack.push_back(b);
        }

        if (m.command == MunolaCommand::Sharp)
        {
            evalMunola(stack);
            Munola b = stack.back();
            stack.pop_back();
            b.pitch += 1;
            stack.push_back(b);
        }

        if (m.command == MunolaCommand::Flat)
        {
            evalMunola(stack);
            Munola b = stack.back();
            stack.pop_back();
            b.pitch -= 1;
            stack.push_back(b);
        }

        if (m.command == MunolaCommand::EndPhrase)
        {
            evalMunola(stack);
            Munola b = stack.back();
            stack.pop_back();
            float d = b.duration;
            b.duration *= 0.75;
            Munola r;
            r.type = MunolaType::Rest;
            r.duration = d - b.duration;
            stack.push_back(r);
            stack.push_back(b);
        }
    }

    if (m.type == MunolaType::Function)
    {
        std::vector<Munola> newStack;
        evalMunola(stack);

        while (!stack.empty())
        {
            Munola b = stack.back();
            stack.pop_back();

            if (b.type == MunolaType::Note)
            {
                float oldDuration = b.duration;
                b.duration = 0.1;

                Munola r;
                r.type = MunolaType::Rest;
                r.duration = oldDuration - b.duration;
                newStack.push_back(r);
                newStack.push_back(b);
            }
            else
            {
                newStack.push_back(b);
            }
        }

        stack = newStack;
    }
}

std::vector<Munola> parseMunola(std::string text)
{
    std::vector<Munola> stack;

    for (int i = 0; i < text.length(); i++)
    {
        if (text.substr(i, 8) == "staccato")
        {
            i += 8;
            Munola m;
            m.type = MunolaType::Function;
            m.function = "staccato";
            stack.push_back(m);
            printf("Staccato\n");
        }
        else if (text[i] >= 'A' && text[i] <= 'G')
        {
            int pitch = 0;
            
            switch (text[i])
            {
            case 'C': pitch = 0; break;
            case 'D': pitch = 2; break;
            case 'E': pitch = 4; break;
            case 'F': pitch = 5; break;
            case 'G': pitch = 7; break;
            case 'A': pitch = 9; break;
            case 'B': pitch = 11; break;
            default:
                break;
            }

            Munola m;
            m.type = MunolaType::Note;
            m.pitch = pitch;
            m.duration = 1.0;
            stack.push_back(m);
        }
        else if (text[i] >= '0' && text[i] <= '9')
        {
            int octave = text[i] - '0';

            Munola m;
            m.type = MunolaType::Command;
            m.command = MunolaCommand::Octave;
            m.octave = octave;
            stack.push_back(m);
        }
        else if (text[i] == '+')
        {
            Munola m;
            m.type = MunolaType::Command;
            m.command = MunolaCommand::Double;
            stack.push_back(m);
        }
        else if (text[i] == '-')
        {
            Munola m;
            m.type = MunolaType::Command;
            m.command = MunolaCommand::Halve;
            stack.push_back(m);
        }
        else if (text[i] == '~')
        {
            Munola m;
            m.type = MunolaType::Command;
            m.command = MunolaCommand::EndPhrase;
            stack.push_back(m);
        }
        else if (text[i] == '#')
        {
            Munola m;
            m.type = MunolaType::Command;
            m.command = MunolaCommand::Sharp;
            stack.push_back(m);
        }
        else if (text[i] == 'b')
        {
            Munola m;
            m.type = MunolaType::Command;
            m.command = MunolaCommand::Flat;
            stack.push_back(m);
        }
    }

    std::reverse(stack.begin(), stack.end());
    evalMunola(stack);
    std::reverse(stack.begin(), stack.end());
    
    Munola m;
    m.type = MunolaType::End;
    stack.push_back(m);

    return stack;
}