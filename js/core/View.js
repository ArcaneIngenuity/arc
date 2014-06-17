View = function()
{
	this.app = undefined;
	this.model = undefined;
	this.enabled = true; //used by timing mechanism to know whether to update or not
	this.parent = undefined;
	this.children = []; //in draw order!
	this.root = undefined; //TODO remove?
	this.body = undefined;//pixel or 3D geometry, typically Box2 or 3 - explicitly injected after view construction... could be bitmap or arbitrary tri mesh
	this.bounds = undefined; //always necessary for first pass pointer detection, and pointer transformations -- may be defined by body
	//TODO draggability bounds (e.g. top bar, corner grip)
	
	this.enable = function()
	{
		/* ABSTRACT: OVERRIDE ME for retained mode render elements */
	}
	
	this.disable = function()
	{
		/* ABSTRACT: OVERRIDE ME for retained mode render elements */
	}
	
	this.onFocusGained = function()
	{
		/* ABSTRACT: OVERRIDE ME */
		//typically, gain foreground
	}

	this.onFocusLost = function()
	{
		/* ABSTRACT: OVERRIDE ME */
		//typically, lose foreground
	}
	
	this.onEntered = function()
	{
		/* ABSTRACT: OVERRIDE ME */
		console.log('entered', this);
	}
	
	this.onExited = function()
	{
		/* ABSTRACT: OVERRIDE ME */
		console.log('exited', this);
	}
	
	this.start = function()
	{
		/* ABSTRACT: OVERRIDE ME */
		if (app.DEBUG)
			console.warn(this.app.phaser.phase.name+"'s View.start() is not implemented.");
	}
	
	this.finish = function()
	{
		//console.log(this.app);
		/* ABSTRACT: OVERRIDE ME */
		if (app.DEBUG)
			console.warn(this.app.phaser.phase.name+"'s View.finish() is not implemented.");
	}
	
	/** Occurs before recursive, collective update of subviews. Use this if you want to simply overdraw all views as in a windowed UI using ordinary blitting. **/
	this.input = function(deltaSec)
	{
		/* ABSTRACT: OVERRIDE ME */
		if (app.DEBUG)
			console.warn(this.app.phaser.phase.name+"'s View.input() is not implemented.");
	}
	
	/** Occurs after recursive, collective update of subviews. Use this if you used update(pre) to gather information onto a single canvas, and want to render that canvas in retrospect. **/
	/*
	this.inputPost = function(deltaSec, inputMaps)
	{
		// ABSTRACT: OVERRIDE ME 
		if (app.DEBUG)
			console.warn(this.app.phaser.phase.name+"'s View.inputPost() is not implemented.");
	}
	*/
	this.output = function(deltaSec)
	{
		/* ABSTRACT: OVERRIDE ME */
		if (app.DEBUG)
			console.warn(this.app.phaser.phase.name+"'s View.output() is not implemented.");
	}

	/** Occurs after recursive, collective update of subviews. Use this if you used update(pre) to gather information onto a single canvas, and want to render that canvas in retrospect. **/
	this.outputPost = function(deltaSec)
	{
		/* ABSTRACT: OVERRIDE ME */
		if (app.DEBUG)
			console.warn(this.app.phaser.phase.name+"'s View.outputPost() is not implemented.");
	}
	
	//only focus must be able to change order!
	this.bringToFore = function(child)
	{
		var index = this.children.indexOf(child);
		if (index > -1)
		{
			var temp = children[i];
			children[i] = children[j];
			children[j] = temp;
		}
	}
	
	this.isRoot = function() //final
	{
		return this == this.root;
	}
	
	this.addChild = function(childView) //final
	{
		this.children.push(childView);
		childView.parent = this;
		childView.root = this.root;
		childView.app = this.app;
	}
	
	this.hasChildren = function()
	{
		return this.children.length > 0;
	}
	
	/*
	this.inputRecurse = function(deltaSec, inputMaps) //final
	{
		this.input(deltaSec, inputMaps);
	
		var children = this.children;
		var length = children.length;
		
		for (var i = 0; i < length; i++)
		{
			//TODO if child had "focus" (in whatever way focus is applied for that input: we should have hover focus and explicit select focus)
			var child = children[i];
			child.inputRecurse(deltaSec, inputMaps);
		}
		
		this.inputPost(deltaSec, inputMaps);
	}
	*/
	
	this.outputRecurse = function(deltaSec) //final
	{
		this.output(deltaSec);
	
		var children = this.children;
		var length = children.length;
		for (var i = 0; i < length; i++)
		{
			var child = children[i];
			child.outputRecurse(deltaSec);
		}
		
		this.outputPost(deltaSec);
	}
	
	this.getDepth = function()
	{
		var depth = 0;

		var ancestor = this.parent;
		while (ancestor)
		{
			depth++;
			ancestor = ancestor.parent;
		}
		
		return depth;
	}
	
	this.getLeaves = function()
	{
		//DFS
		var leaves = [];
		if (this.children.length == 0)
			leaves.push(this);
		else
		{
			for (var i = 0; i < this.children.length; i++)
			{
				var child = this.children[i];
				var childLeaves = child.getLeaves();
				leaves.extend(childLeaves); //add into array to be returned
			}
		}
		return leaves;
	}
	
	this.getLeavesEnabled = function()
	{
		//DFS
		var leaves = [];
		
		if (this.enabled)
		{
			if (this.children.length == 0)
				leaves.push(this);
			else
			{
				for (var i = 0; i < this.children.length; i++)
				{
					var child = this.children[i];
					var childLeaves = child.getLeaves();
					leaves.extend(childLeaves); //add into array to be returned
				}
			}
		}
		return leaves;
	}
	
	//--- transformation functions -- later to go in Transform abstract class (2D or 3D) ---//
	
	//remember to clone before running values through these functions
	//position is in local coordinates for this object.
	this.toChild = function(position, child)
	{
		position.x -= child.bounds.x0;
		position.y -= child.bounds.y0;
	}
	
	//position is in local coordinates for child object.
	this.fromChild = function(position, child)
	{
		position.x += child.bounds.x0;
		position.y += child.bounds.y0;
	}
	
	//position is in local coordinates for this object.
	this.toParent = function(position)
	{
		position.x += this.bounds.x0;
		position.y += this.bounds.y0;
	}

	//position is in local coordinates for parent object.
	this.fromParent = function(position)
	{
		position.x -= this.bounds.x0;
		position.y -= this.bounds.y0;
	}
	
	//position is in world coordinates.
	this.fromWorld = function(position)
	{	
		var path = [];
		var view = this; //could be root, otherwise...
		path.push(view);
		while (view.parent)
		{
			view = view.parent; //...move up to root.
			path.unshift(view); //root down order
		}
		
		//walk down, transforming
		for (var i = 0; i < path.length; i++)
		{
			var view = path[i];
			position.x -= view.bounds.x0;
			position.y -= view.bounds.y0;
			//console.log('++', i, position, view.bounds.x0);
		}
	}
	
	//position is in local coordinates for this.
	this.toWorld = function(position, parent)
	{
		var view = this; //could be root, otherwise...
		while (view.parent)
		{
			view = view.parent; //...move up to root.
			position.x += view.bounds.x0;
			position.y += view.bounds.y0;
		}
	}
	
	this.enable = function()
	{
		/* ABSTRACT: OVERRIDE ME */
	}
	
	this.disable = function()
	{
		/* ABSTRACT: OVERRIDE ME */
	}
}

