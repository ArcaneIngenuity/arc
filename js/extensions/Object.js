//Messes with overridden .clone() methods -- in non-JS, these are all custom-defined anyway, so may as well do same.
/*
var clone = function() {
    var newObj = (this instanceof Array) ? [] : {};
    for (var i in this) {
        if (this[i] && typeof this[i] == "object") {
            newObj[i] = this[i].clone();
        }
        else
        {
            newObj[i] = this[i];
        }
    }
    return newObj;
}; 

Object.defineProperty( Object.prototype, "clone", {value: clone, enumerable: false});
*/