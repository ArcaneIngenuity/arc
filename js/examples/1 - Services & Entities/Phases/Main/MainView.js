MainView = function(app, model)
{
	View.call(this, app, model); //extend base framework class

	this.bounds = new Box2();
	console.log(this.app);
	var monsterService = this.app.services.get(SERVICE_MONSTER);

	this.monstersDOM = undefined;
	this.thoughtBubble = undefined;
	
	//--- abstract framework methods ---//
	this.start = function()
	{
		var dom = this.dom;
		var model = this.model;
		model.arenaWidth  = dom.clientWidth;
		model.arenaHeight = dom.clientHeight;
		
		this.monstersDOM		= document.getElementById('Monsters');
		this.thoughtBubbleDOM 	= document.getElementById('ThoughtBubble');
		console.log('mnstr', this.monstersDOM);
		console.log('thght', this.thoughtBubbleDOM);
		console.log(this.children);
		console.log(dom.children);

		//listeners for DOM events may be set up in View.start()
		window.addEventListener( 'resize', this.onWindowResize.bind(this), true );
		
		//fire up the engines!
		this.onWindowResize();
		this.show();		
	}
	
	this.finish = function()
	{
		var dom = this.dom;
	}
	
	this.input = function(deltaSec)
	{
		var dom = this.dom;
		var model = this.model;
	
		//get InputHub & Inputs containing InputChannels
		var input = this.app.input;
		var mouse = input.array[INPUT_MOUSE];
		var keyboard = input.array[INPUT_KEYBOARD];
		
		//get Pointer
		var pointer = this.app.pointer;
		
		//***TODO convert raw input to (App or Phase) Model state and View state.
		if (mouse.channels[MOUSE_BUTTON_LEFT].delta > 0)// && !this.loaded)
		{
		
			console.log( 'add' );
			
			var monsterModel = this.addNewMonster();
		}
		
		if (mouse.channels[MOUSE_BUTTON_RIGHT].delta > 0)
			console.log( 'remove' );
	}
	
	this.output = function(deltaSec)
	{
		var dom = this.dom;
		var model = this.model;
		var monsters = model.monsters;
		var monstersDOM = this.monstersDOM;
		var length = monstersDOM.children.length;

		var o = {};
		n+= 0.1
		for (var m = 0; m < length; m++)
		{
			var monsterDOM = monstersDOM.children[m];
			var monster = monsterDOM.model;
			var style = monsterDOM.style;
			style.left = Math.floor(monster.x) + 'px';
			style.top  = Math.floor(monster.y) + 'px';
		}
	}
	
	this.show = function()
	{
		var dom = this.dom;
		var style = dom.style;
		style.visibility = "visible";
		style.display = "block";
	}
	
	this.hide = function()
	{
		var dom = this.dom;
		var style = dom.style;
		style.visibility = "hidden";
		style.display = "none";
	}
	
	//--- helper methods ---//
	this.onWindowResize = function()
	{
		//assumes a 2D view (standard)
		var dom = this.dom;
		var width = dom.clientWidth;
		var height = dom.clientHeight;
		this.bounds.setWidth(width);
		this.bounds.setHeight(height);
		
		//***TODO (2D) anything else that needs to change, such as UI-reflow, on resize.
		//***TODO (3D) include any logic which adjusts accordingly, e.g. changing camera aspect ratio.
	}
	
	/** Calls to service, model, and View -- all of which are standard for a View input handler like this */
	this.addNewMonster = function()
	{
		var dom = this.dom;
		var monstersDOM = this.monstersDOM;
		var model = this.model;
		var monsters = this.model.monsters;
		var monster = new MonsterModel();
		monster.index = monsterService.generateIndex();
		monster.name = monsterService.generateName(monster.index);
		monster.x = Math.floor(Math.random() * dom.clientWidth);
		monster.y = Math.floor(Math.random() * dom.clientHeight);
		monster.scale = 0.5 + 2.5 * Math.random();
		monster.width = 80;
		monster.height = 80;
		monster.speed = 20 + 20 * monster.scale + 20 * Math.random(); //pixel per sec
		model.monsters.push(monster);
			
		var url = monsterService.getImageURL(monster.name);
	
		var image = new Image();
		image.crossOrigin = 'Anonymous';
		
		image.addEventListener("load", function(event)
		{
			var imageLoaded = event.target;

			//first get unskewed with transparent background
			var canvasWorking = document.createElement('canvas');//document.getElementsByTagName("canvas")[0];
			var ctxWorking = canvasWorking.getContext("2d");
			canvasWorking.width = imageLoaded.width; //enough space for angled shadow
			canvasWorking.height = imageLoaded.height;
			ctxWorking.drawImage(imageLoaded, 0, 0);

			var imageData = ctxWorking.getImageData(0, 0, imageLoaded.width, imageLoaded.height);
			var pixels = imageData.data;
			var corner = pixels[0];
			var cornerColour = {r: undefined, g: undefined, b: undefined};
			cornerColour.r = pixels[0];
			cornerColour.g = pixels[1];
			cornerColour.b = pixels[2];
			
			for (var i = 0; i < pixels.length; i += 4)
			{
				var r = pixels[i];
				var g = pixels[i + 1];
				var b = pixels[i + 2];
				var a = pixels[i + 3];

				if (r === cornerColour.r && g === cornerColour.g && b === cornerColour.b)
				{
					pixels[i + 3] = 0;
				}
			}
			ctxWorking.putImageData(imageData, 0, 0);
			
			//final canvas
			var canvas = document.createElement('canvas');//document.getElementsByTagName("canvas")[0];
			var ctx = canvas.getContext("2d");

			//draw skewed shadow
			canvas.width = imageLoaded.width * 2; //enough space for angled shadow
			canvas.height = imageLoaded.height;
			ctx.transform(1,0,-1,1,80,0);
			ctx.drawImage(canvasWorking, 0, 0);
			
			imageData = ctx.getImageData(0, 0, imageLoaded.width * 2, imageLoaded.height);
			pixels = imageData.data;
			
			//skewed pixels to shadow colour
			for (var i = 0; i < pixels.length; i += 4)
			{
				var r = pixels[i];
				var g = pixels[i + 1];
				var b = pixels[i + 2];
				var a = pixels[i + 3];

				if (a != 0)
				{
					pixels[i] = 
					pixels[i + 1] = 
					pixels[i + 2] = 0;
					pixels[i + 3] = 127;
				}
			}
			
			ctx.putImageData(imageData, 0, 0);
			ctx.transform(1,0,1,1,-80,0);
			ctx.drawImage(canvasWorking, 0, 0);
			
			canvas.style.transform = 		'scale('+monster.scale+', '+monster.scale+')';
			canvas.style.webkitTransform = 	'scale('+monster.scale+', '+monster.scale+')';
			canvas.style.msTransform = 		'scale('+monster.scale+', '+monster.scale+')';
			
			canvas.style.left = (-monster.width / 4) + 'px';
			canvas.style.top = (-monster.height / 2) + 'px';
			
			var monsterDOM = canvas;
			
			//var monsterDOM = document.createElement('div');
			//monsterDOM.className = 'monster';
			//monsterDOM.appendChild(canvas);
			
			//var bub = new Image()
			//bub.src = './Assets/ThoughtBubble.png';
			//bub.model = monster;
			//bub.className = 'monster';
			//var monsterDOM = bub;
			
			monsterDOM.className = 'monster';
			monsterDOM.model = monster;
			monstersDOM.appendChild(monsterDOM); //will be at monster.index as there are no children to this element other than monsters.
		});
		
		image.src = url;
			
		return monster;
	}
}

MainView.prototype = Object.create(View.prototype);
MainView.prototype.constructor = MainView;