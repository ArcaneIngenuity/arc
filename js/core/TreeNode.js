//used in for the nesting of UI elements.
TreeNode = function()
{
	this.parent = undefined;
	this.children = undefined;
	this.data = undefined; //could be anything. for UI input (in C), it's the bounds. for Views, it's the View which refs same bounds.
}

//node->View->bounds... used for both calculating UI input, AND View rendering. No ref to UI input classes means no encapsulation concerns.
//node->View->position