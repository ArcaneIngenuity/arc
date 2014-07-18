/*
dj.array.clear = function(array) //use only for sparse arrays / lists
{
	array.length = 0; //most efficient.
}
*/

/*
Array.prototype.clear = function() //use only for sparse arrays / lists
{
	this.length = 0; //most efficient.
}
*/
/*
Array.prototype.removeAtIndex = function(index) //use only for sparse arrays / lists
{
	this.splice(index, 1);
}
*/
/*
//add one array's elements to the end of another array
Array.prototype.extend = function (other_array)
{
    // you should include a test to check whether other_array really is an array
    other_array.forEach(function(v) {this.push(v)}, this);    
}
*/
/*
Array.prototype.remove = function(element)
{
	var result = false;
	//generalised more for C
	for (var i in this)
	{
		var elementOther = this[i];
		if (element === elementOther)
		{		
			this.splice(i, 1);
			result = true;
			break;
		}
	}
	return result;

	//does not work in ie < 9
	//var index = array.indexOf(5);
	//if (index > -1) {
	//	array.splice(index, 1);
	//}
	
}
*/

/*
Array.append = function(array, element)
{
	var result = false;
	//generalised more for C
	for (var i in array)
	{
		var elementOther = array[i];
		if (element === elementOther)
		{		
			array.splice(i, 1);
			result = true;
			break;
		}
	}
	return result;
}
*/
