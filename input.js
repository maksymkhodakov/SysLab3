function createQuote(quote, callback){
    const myQuote = "Like I always say, " + quote;
    callback(myQuote); // 2
}

function logQuote(quote){
    console.log(quote);
}

createQuote("eat your vegetables!", logQuote); // 1

console.log(0xb1)
console.log(123)
console.log(123.123)

// Result in console:
// Like I always say, eat your vegetables!