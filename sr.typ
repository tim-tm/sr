#import "@preview/cetz:0.5.2"

#set page(
  paper: "a4",
  header: align(right)[
  ],
  numbering: "1",
)
#set par(justify: true)
#set text(size: 11pt, lang: "en")
#set heading(numbering: "1.1.1")

#show title: set text(size: 17pt)
#show title: set align(center)

#title[
  Software rendering
]
#align(center)[
  Tim Teichmann
]

This document will guide you through my journey of exploring software rendering.
It serves as an addition to the actual implementation inside of this repository.
For the most part, it will include my own derivations of different formulas.

= 3D projection
As of writing this chapter,
I am not yet sure about the exact direction this project will be heading to.
Whether I want to write a simple Blender-clone or develop a game,
I will surely need a third dimension.

Consider the following figure:
#let project(camera, point) = (
  (camera.at(0) * point.at(2) - camera.at(2) * point.at(0)) / (point.at(2) - camera.at(2)),
  (camera.at(1) * point.at(2) - camera.at(2) * point.at(1)) / (point.at(2) - camera.at(2)),
  0,
)

#figure(
  cetz.canvas({
    import cetz.draw: *

    perspective(
      x: 15deg,
      y: 120deg,
      z: 0deg,
      {
        line((-5, 0, 0), (5, 0, 0))
        mark((-5, 0, 0), (-6, 0, 0), symbol: ">", fill: black)
        mark((5, 0, 0), (6, 0, 0), symbol: ">", fill: black)
        content((6, 0, 0), $+x$)
        content((-6, 0, 0), $-x$)

        line((0, -5, 0), (0, 5, 0))
        mark((0, -5, 0), (0, -6, 0), symbol: ">", fill: black)
        mark((0, 5, 0), (0, 6, 0), symbol: ">", fill: black)
        content((0, 6, 0), $+y$)
        content((0, -6, 0), $-y$)

        line((0, 0, -5), (0, 0, 5))
        mark((0, 0, -5), (0, 0, -6), symbol: ">", fill: black)
        mark((0, 0, 5), (0, 0, 6), symbol: ">", fill: black)
        content((0, 0, 6), $+z$)
        content((0, 0, -6), $-z$)

        rect((0, 0, 0), (4, 3, 0), stroke: black.transparentize(90%), fill: black.transparentize(90%))

        let camera = (2, 1.5, -2)
        circle(camera, radius: (0.03, 0.03))
        content((camera.at(0), camera.at(1) - 0.2, camera.at(2)), $C$)

        let points = ((pos: (2, 2, 4), name: $A$), (pos: (4, 1, 2), name: $B$))
        for (pos, name) in points {
          let on_screen = project(camera, pos)
          circle(on_screen, radius: (0.03, 0.03))
          circle(pos, radius: (0.03, 0.03))
          content((pos.at(0), pos.at(1) - 0.3, pos.at(2)), name)
          line(camera, pos, stroke: black.transparentize(70%))
        }
      },
    )
  }),
  caption: [Visualization of the first projection approach],
)

The point $C$ is supposed to be the camera looking at the points $A,B$.
The grey plane aims to simulate a screen.
Let's call $w$ the width and $h$ the height of that screen.
Points $A$ and $B$ will be elements of the following set:
$
  P = {vec(x, y, z) in RR^3 | 0 <= x <= w and 0 <= y <= h and z >= 0}
$
In order to find the screen coordinates of a given point $v in P$,
we will have to construct the equation $f: RR -> RR^3, t |-> c + t u = c + t(v-c)$ of the line between $v$ and our camera $c$
and find $t in RR$ where $f(t)=vec(x, y, 0)$.
Let's solve:
$
      & vec(x, y, 0) = c + t(v-c) \
  <=> & vec(x, y, 0) = vec(c_1, c_2, c_3) + t(vec(v_1, v_2, v_3) - vec(c_1, c_2, c_3)) \
$
We can solve the last equation for $t$:
$
      & 0 = c_3 + t(v_3 - c_3) \
  <=> & -c_3 = t(v_3 - c_3) \
  <=> & t = -frac(c_3, v_3 - c_3)
$
It's fine to divide by $v_3 - c_3$ as long as $v_3 != c_3$.
$
  vec(x, y, 0) = & vec(c_1, c_2, c_3) + t(vec(v_1, v_2, v_3) - vec(c_1, c_2, c_3)) \
               = & vec(c_1, c_2, c_3) - (frac(c_3, v_3 - c_3)) (vec(v_1, v_2, v_3) - vec(c_1, c_2, c_3)) \
               = & frac(1, v_3 - c_3) vec(c_1 v_3 - c_3 v_1, c_2 v_3 - c_3 v_2, 0)
$

Take a look at the following cube I drew using this formula:
#let parse_obj(content) = {
  let result = (
    vertices: (),
    faces: (),
    vert_max: (-calc.pow(2, 31), -calc.pow(2, 31), -calc.pow(2, 31)),
    vert_min: (calc.pow(2, 31), calc.pow(2, 31), calc.pow(2, 31)),
  )
  for line in content.split("\n") {
    let splt = line.split(" ")
    if splt.len() != 4 {
      continue
    }
    let (typ, x, y, z) = splt
    if typ == "v" {
      let v = (float(x), float(y), float(z))
      for i in range(3) {
        if v.at(i) > result.vert_max.at(i) {
          result.vert_max.at(i) = v.at(i)
        }
        if v.at(i) < result.vert_min.at(i) {
          result.vert_min.at(i) = v.at(i)
        }
      }
      result.vertices.push(v)
    } else if (typ == "f") {
      let v = (int(x), int(y), int(z))
      result.faces.push(v)
    }
  }

  return result
}

#figure(
  image("res/sr_fig_1.png", width: 6cm, height: 4.5cm),
  caption: [Example of the first projection approach],
)
