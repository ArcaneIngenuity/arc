Disjunction.Core.View = function()
{
	this.enabled = true; //used by timing mechanism to know whether to update or not
	this.parent = undefined;
	this.children = []; //in draw order!
	//this.childrenByName = {}; //should be private
	this.childrenByClassName = {}; //should be private
	this.root = undefined; //TODO remove?
	this.body = undefined; //pixel or 3D geometry, typically Box2 or 3 - explicitly injected after view construction... could be bitmap or arbitrary tri mesh
	this.bounds = undefined; //always necessary for first pass pointer detection, and pointer transformations -- may be defined by body

	//ABSTRACT METHODS
	/** Used to set up resources or values specific to this View. */ 
	this.start = function()
	{
		/* ABSTRACT: OVERRIDE */
	}
	
	/** Used to clean up resources or reset values for this View, if it is no longer needed and can be released. */ 
	this.stop = function()
	{
		/* ABSTRACT: OVERRIDE */
	}
	
	/** (When focused) Process all input into this View and modify model and View state (if using a persistent / retained mode display list) accordingly, if View enabled. **/
	/*
	this.input = function()//app, model)
	{
		//ABSTRACT: OVERRIDE
		//-run only for the currently focused view in a given global update
	}
*/
	/** Render all output for this View based on model state, on View tree walk-down, if View enabled. **/
	this.output = function()//app, model)
	{
		//ABSTRACT: OVERRIDE
	}

	/** Render all output for this View based on model state, on View tree walk-up, if View enabled. **/
	/** Occurs after recursive, collective update of subviews. Use this if you used update(pre) to gather information onto a single canvas, and want to render that canvas in retrospect. **/
	this.outputPost = function(app, model)
	{
		//ABSTRACT: OVERRIDE
	}
	
	//restores visibility.
	this.show = function()
	{
		/* ABSTRACT: OVERRIDE */
	}
	
	//keeps view processing even if its not visible.
	this.hide = function()
	{
		/* ABSTRACT: OVERRIDE */
	}
	
	this.dispose = function()
	{
		//ABSTRACT
	}
	
	//FINAL METHODS
	//re-enables view processing.
	this.enable = function()
	{
		this.enabled = true;
		this.show();
	}
	
	//stops view processing altogether.
	this.disable = function()
	{
		this.enabled = false;
		this.hide();
	}
	
	this.outputRecurse = function(deltaSec) //final
	{
		this.output(deltaSec);
	
		var children = this.children;
		if (children)
		{
			var length = children.length;
			for (var i = 0; i < length; i++)
			{
				var child = children[i];
				if (child.enabled)
					child.outputRecurse(deltaSec);
			}
		}
		
		this.outputPost(deltaSec);
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
		//this.childrenById[childView.id] = childView;
		var classNamesJoined = childView.dom.className;
		if (classNamesJoined.length > 0)
		{		
			var classNames = classNamesJoined.split(' ');
			var className = classNames[0];
			
			//create array in map, if it doesn't already exist.
			if (!this.childrenByClassName.hasOwnProperty(className))
				this.childrenByClassName[className] = [];
				
			var array = this.childrenByClassName[className];
			
			array.push(childView);
		}
		
		childView.parent = this;
		childView.root = this.root;
		childView.app = this.app;
		childView.model = this.model;
	}
	/*
	this.removeChild = function(childView) //final
	{
		this.children.push(childView);
		this.childrenByName[childView.id] = undefined;
		childView.parent = undefined;
		childView.root = undefined;
		childView.app = undefined;
		childView.model = undefined;
	}
	*/
	
	//C-compatible array search to find child by id.
	//TODO use a hashmap? ...external access remains exactly the same.
	this.getChildById = function(id)
	{
		//return this.childrenByName[id];
		var children = this.children;
		var length = children.length;
		for (var i = 0; i < length; i++)
		{
			var child = children[i];
			if (child.id === id)
			{
				return child;
			}
		}
		
		return undefined;
	}
	
	this.getChildrenByClassName = function(className)
	{
		return this.childrenByClassName[className];
	}
	
	this.hasChildren = function()
	{
		return this.children.length > 0;
	}
	
	this.startRecurse = function() //final
	{
		var children = this.children;
		if (children)
		{
			var length = children.length;
			for (var i = 0; i < length; i++)
			{
				var child = children[i];
				child.startRecurse();
			}
		}
		
		this.start(); //create from bottom up
	}
	
	this.stopRecurse = function(deltaSec) //final
	{
		this.stop();
	
		var children = this.children;
		if (children)
		{
			var length = children.length;
			for (var i = 0; i < length; i++)
			{
				var child = children[i];
				child.stopRecurse();
			}
		}
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
	
	this.isLeaf = function()
	{
		return this.children == undefined;
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

	this.focus = function()
	{
		this.app.focus = this;
	}
	
	//JS - should be in extensions?
	
	//from http://bradsknutson.com/blog/javascript-detect-mouse-leaving-browser-window/
	this.addDOMEvent = function(obj, evt, fn)
	{
		if (obj.addEventListener) {
			obj.addEventListener(evt, fn, false);
		}
		else if (obj.attachEvent) {
			obj.attachEvent("on" + evt, fn);
		}
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.View = Disjunction.Core.View;
