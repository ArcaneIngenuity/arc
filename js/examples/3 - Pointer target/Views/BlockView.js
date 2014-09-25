BlockView = function()
{
	View.call(this); //extend base framework class

	this.bounds = new Box2();

	//--- abstract framework methods ---//
	this.start = function() 
	{
		var dom = this.dom;
		dom.style.display = 'block';
		
		//listeners for DOM events may be set up in View.start()
		window.addEventListener( 'resize', this.onWindowResize.bind(this), true );
		
		//fire up the engines!
		this.onWindowResize();
		this.show();
	}
	
	this.setup2D = function(dom)
	{
	}

	this.finish = function()
	{
		var dom = this.dom;
	}
	
	this.input = function(deltaSec)
	{
		var dom = this.dom;
		var pointer = disjunction.pointer;
		
		if (pointer.selectChannel.delta > 0)
		{
			console.log(pointer.target);
			
			var parent = dom.parentNode;
			var grandparent = dom.parentNode.parentNode;
		
			var target = document.getElementsByClassName('target')[0];
			var targetParent = document.getElementsByClassName('targetParent')[0];
			var targetGrandparent = document.getElementsByClassName('targetGrandparent')[0];
			
			if (target)
				target.classList.remove('target');
			if (targetParent)
				targetParent.classList.remove('targetParent');
			if (targetGrandparent)
				targetGrandparent.classList.remove('targetGrandparent')
			
			dom.classList.add('target');
			if (parent.view instanceof BlockView)
				parent.classList.add('targetParent');
			if (grandparent.view instanceof BlockView)
				grandparent.classList.add('targetGrandparent');
		}	
	}
	
	
	this.output = function(deltaSec)
	{
		var dom = this.dom;
		var model = this.model.model;
	}
	
	this.outputPost = function(deltaSec)
	{
		var dom = this.dom;
		var model = this.model.model;
		
		//***TODO render using Phases's Model and View (this) state; always prefer use of output() unless you have special requirements.
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
		var dom = this.dom;
		var domRect = dom.getBoundingClientRect();
		this.bounds.x0 = domRect.left;
		this.bounds.y0 = domRect.top;
		this.bounds.setWidth(domRect.width);
		this.bounds.setHeight(domRect.height);
		console.log(dom.className, domRect, this.bounds);
	}
	
	this.getAspectRatio = function()
	{
		return window.innerWidth / window.innerHeight;
	}
}

MainView.prototype = Object.create(View.prototype);
MainView.prototype.constructor = MainView;