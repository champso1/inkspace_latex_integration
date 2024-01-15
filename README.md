# Inkspace Latex Integration

## Intro
This was a super small little project I created after watching [this](https://www.youtube.com/watch?v=DOtM1mrWjUo&ab_channel=SeniorMars) YouTube video, and reading the [corresponding blog](https://castel.dev/post/lecture-notes-1/) by Gilles Castel. I am getting more interested in LaTeX as of recently, and I am looking to start taking notes in it. Even if not during class, translating my handwritten notes into clean, neat LaTeX is quite enticing. As such, a nice way to do graphics has eluded me as a physics major, since there are a lot of complicated figures to work with!

## Features
There are a number of things that I can do with this. I have a set folder where I keep a bunch of template svg's, and the program links to it. From there, I can add a template to the project I'm working on, through the command:
```
.\graphic --get <template name> <new name>
```
Where `new name` is what I want to call the template when it is copied to my project. From there, the program opens that file Inkspace where I can edit.

I can also edit other figures that I have added to my project, as well as call a customized save command which skips all the GUI dialog nonsense and immediately generates a `.pdf_tex` file which I can then insert into my LaTeX file.

In order to nicely do that, I have added the exact code from the blog to my preamble:

```latex
\usepackage{import}
\usepackage{xifthen}
\usepackage{pdfpages}
\usepackage{transparent}

\newcommand{\incfig}[1]{
    \def\svgwidth{\columnwidth}
    \import{./figures/}{#1.pdf_tex}
}
```

And, with a VS Code snippet, I can easily insert the required code to insert the figure (also the exact code from the blog):

```latex
    \begin{figure}[ht]
        \centering
        \incfig{fig-name}
        \caption{caption}
        \label{fig:fig-name}
    \end{figure}
```


## TODO
Implement a few more nice features, like the ability to create templates with certain parameters.

The main thing I want to eventually implement is similar to how the actual VIM plugin works (I think, anyway), in that there is an event listener for when the file is saved which immediately calls the corresponding custom save script that generates the `.pdf_tex` file. Not that it's a huge deal, all my program requires is that switch over to your terminal and press the up arrow a couple times then enter, but it would be nice (and it would be a fantastic way to learn more about the Windows C API, which is nuts).