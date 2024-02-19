// npx or npm install canvas-sketch
// npx canvas-sketch-cli sketch02.js
// more info: https://github.com/mattdesl/canvas-sketch

const canvasSketch = require('canvas-sketch');
const math = require('canvas-sketch-util/math');
const random = require('canvas-sketch-util/random');

const settings = {
  dimensions: [ 3240, 3240 ]
};

function formula_x(width,height,i) {
  return width/2/2 * Math.sin(i/9.507)+width/10*Math.cos(i/19.01);
}

function formula_y(width,height,i) {
  return height/2/2 * Math.cos(i/19.1)+height/10*Math.cos(i/19.01)+height/10*Math.cos(i/19.0);
}

function extra_formula_x(x) {
  return 10*Math.sin(x*0.01)+x;
}

const sketch = () => {
  return ({ context, width, height }) => {
    // Setup the context
    context.fillStyle = 'white';
    context.fillRect(0, 0, width, height);
    context.lineWidth=10;
    context.beginPath();
    context.rect(0,0,width,height);
    context.stroke();
    context.strokeStyle = 'black';
    context.lineWidth = 2;
    context.translate(width/2,height/2);
    context.scale(1, -1); // invert y axis to match arduino coordinates
    for (let i=0;i<30000;i++) {

      var startX= formula_x(width,height,i);
      var endX=   formula_x(width,height,i+1);
      var startY= formula_y(width,height,i);
      var endY=   formula_y(width,height,i+1);

      context.beginPath();
      context.moveTo(startX, startY); // Move to the start point
      context.lineTo(endX, endY); // Draw a line to the end point
      context.stroke(); // Draw that line
    }
  };
};

canvasSketch(sketch, settings);


//////////////  Some cool drawings  //////////////////

/* 
// 8000 iterations (on A4)
function formulaX(width,height,i) {
  return width/2/2 * Math.sin(i/100.1)+width/2/5*Math.cos(i/12.003);
}
function formulaY(width,height,i) {
  return height/2/2 * Math.cos(i/100.01)+width/2/5*Math.cos(i/10);
}
*/

/* 
//100000 iterations (on A3)
function formulaX(width,height,i) {
  return width/2/1.5 * Math.sin(i/100)+width/10*Math.cos(i/100);
}
function formulaY(width,height,i) {
  return height/2/2 * Math.cos(i/100.08)+width/10*Math.cos(i/10.254);
}
*/

/* 
//100000 iterations (on A3)
function formulaX(width,height,i) {
  return width/2/1.5 * Math.sin(i/100)+width/10*Math.cos(i/100.03);
}
function formulaY(width,height,i) {
  return height/2/2 * Math.cos(i/99.999)+height/10*Math.cos(i/10.006)+height/10*Math.cos(i/10.009);
}
*/

/*
//40000 iterations (on A3)
function formulaX(width,height,i) {
  return width/2/2 * Math.sin(i/30.1)+width/10*Math.cos(i/10.0);
}
function formulaY(width,height,i) {
  return height/2/2 * Math.cos(i/30.04)+width/10*Math.cos(i/10.02);
}
*/