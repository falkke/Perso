var laps = 5;

function 	randomColor()
{
    var colors = ['#ffffff', '#0000ff', '#ff0000'];
    return colors[Math.floor(Math.random() * colors.length)];
}

class 	confetti
{
	constructor(x, y)
	{
		this.x = x;
		this.y = y;
		this.size = (Math.random() * 0.5 + 0.75) * 15;
		this.gravity = (Math.random() * 0.5 + 0.75) * 0.1;
		this.rotation = (Math.PI * 2) * Math.random();
		this.rotationSpeed = (Math.PI * 2) * (Math.random() - 0.5) * 0.001;
		this.color = randomColor();
	}
}

function	update(list, max)
{
	var fete = document.getElementById("fete");
	var i = list.length - 1;
	
	while (i >= 0)
	{
		if(list[i].y > fete.height)
            list[i].y = 0;
		list[i].y += list[i].gravity * laps;
        list[i].rotation += list[i].rotationSpeed * laps;
		i--;
	}
}

function	draw(list)
{
	var fete = document.getElementById("fete");
	var ctx = fete.getContext('2d');
	var i = list.length - 1;
		
	ctx.clearRect(0, 0, fete.width, fete.height);
	while(i >= 0)
	{
		ctx.save();
        ctx.fillStyle = list[i].color;
        ctx.translate(list[i].x + list[i].size / 2, list[i].y + list[i].size / 2);
        ctx.rotate(list[i].rotation);
        ctx.fillRect(-list[i].size / 2, -list[i].size / 2, list[i].size, list[i].size);
        ctx.restore();
		i--;
	}
}

function	vivapiniata()
{
	var fete = document.getElementById("fete");
	var list = [];
	var max = 200;
	
	while(list.length < max)
		list.push(new confetti(Math.random() * fete.width, Math.random() * fete.height));
	draw(list);
	window.setInterval(function()
	{
		update(list, max);
		draw(list);
	}, laps);
}

/*
function zoomIn()
{
	var lord = document.getElementById("lord");
	lord.style.height = "5000px";
	lord.style.width = "3500px";
}
*/

function onLoad()
{
	var lord = document.getElementById("lord");
	var	song = document.getElementById("song");
	var fete = document.getElementById("fete");
	var ctx = fete.getContext('2d');
	
	song.style.visibility='hidden';
	song.volume = 1;
	//zoomIn();
	
	fete.width = 1920;
	fete.height = 1000;
	ctx.globalAlpha = 0.5;
	vivapiniata();
}

window.addEventListener("load", onLoad);