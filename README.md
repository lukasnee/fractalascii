# fractalASCII

![fractalascii-demo-by-lukasnee](fractalascii-demo.png)

ASCII drawn [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set) terminal navigator. Made with [ncurses](https://en.wikipedia.org/wiki/Ncurses).

# Setup

```shell
$ sudo apt-get install libncurses5-dev libncursesw5-dev
$ git clone https://github.com/lukasnee/fractalascii.git
$ cd fractalascii
$ make fractalascii
$ ./fractalascii
```

# Controls
- `w`, `a`, `s`, `d` - move
- `e` - zoom in 10%
- `q` - zoom out 10%
- `r` - reset camera
- `1`, `2`, `3` ... - interesting views
