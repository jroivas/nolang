# Pure

 - No classes
 - Scope access only passed values
 - Scope can generate new values
 - Scope may return any number of values
 - Data passed as constant to methods
 - Mutability achieved by making copy
 - Strong types
  * Some types can be deduced
 - Avoid side efects by design
 - Pure methods can call only pure methods
 - Side methods can call any methods
 - All memory is cleared before given to application use
 - Namespaces
 - Namespaces may contain only methods and constants
 - No other variables allowed in namescape (global) scope


## Types

 - boolean
 - byte
 - int
    * int8
    * int16
    * int32
    * int64
 - uint
    * Same sizes as int
 - Double / Float
 - Number
    * Any int, uint or floating point type
 - String
 - List
 - Map
 - Closure

## Structures

One can define own types with `struct`

    struct Date {
        milliSecondsSinceEpoch : int64
        negative : boolean
    }

Since Nolang does not support objects, or object methods, structs are as well pure data only.

To generate new instances of structures one can call it's default initializer,
where one passes values to initialize in same order as in definition.
All values in struct are by default initialized as zero, so one can omit some parameters:

    Date(100000000000, false)
    Date(100000000000)
    Date()

Or one can initialize them in custom methods like:

    pure generateDate(year, month, day : int32) =>
        d : Date
        d.negative = year < 1970
        d.milliSecondsSinceEpoch = (year - 1970) * millisecondsInYear
        d.milliSecondsSinceEpoch += month * millisecondsInMonth
        d.milliSecondsSinceEpoch += day * millisecondsInDay
        return d

## Enums

Enums are strictly typed:

    enum Colors {
        black,
        red,
        blue
    }

    enum Bits {
        first = 1,
        second = 2,
        third = 4,
        fourth = 8,
        fifth = 16,
        sixth = 32,
        seventh = 64,
        eight = 128
    }

## Method overloading

Nolang supports method overloading, but with certain limitations.

First of all, Nolang does not have C++ kind of name mangling,
but still supports limited mangling.
All exported symbols are C compatible, and human readable.

Nolang prefers postfixes, and simple cases type is just added as postfix.
Let's see these definitions:

    print(p : int32) => //..
    print(p : int8) => //..
    print(p : Double) => //..
    print(p : String) => //..
    print(p : List) => //..
    print(a : int8, b : Double, c : List) => //..

Nolang would mangle these to (C style):

    print_int32(int32 p)
    print_int8(int8 p)
    print_double(double p)
    print_string(string *p)
    print_list(list *p)
    print_int8_double_list(int8 a, double b, list *c)
    print(list *p)

The last export is the default export in case of method overloading.
This way we can still call for example (pseudocode):

    list args;
    args.addString("Hello ";
    args.addInt32(42);
    args.addString(" and ");
    args.addDouble(3.14159);
    print(&args)

On method overloading case default implementation is to iterate list parameters
and call corresponding type method.

If there's no overloading, the exposed name is the name itself without any postfixes.


## Example

Work in progress example, for more accurate examples, see [examples](examples)

    const MeaningOfTheLife : int32 = 42

    pure generatePerson(name:String, birthDay:Date) : Map =>
        [
            "name": name,
            "birthDate": birthDay
        ]
        // Same as:
        [("name", name), ("birthDate", birthDay)]
        // Same as:
        mapSet(mapSet([], "name", name), "birthDate", birthDay)

    pure timeInYearToDay(startDate:Date) : Double =>
        toYears(currentDate() - startDate)

    pure personWithNewName(person:Map, newName:String) : Map =>
        // Explicit copy, updating person is error
        newPerson = copy(person)
        newPerson.name = newName
        return newPerson

        // Same as:
        mapSet(copy(person), "name", newName)

    pure matchAge(age:Double) =>
        match age =>
            0..2 'infant'
            3..6: 'kid'
            7..12: 'school kid'
            13..18: 'teen'
            19..60: 'adult'
            61..: 'senior'
            ?: 'invalid age'

    pure multiReturn(x:Double,y:Double)  =>
        return x, y

    pure splitArray(x:List) =>
        return head(x), tail(x)

    pure splitArray2nd(x:List) =>
        // First, skip second, take from third forward
        return head(x), tail(x, 2)

    pure splitArrayHalf(x:List) =>
        return half(x), halfRest(x)

    pure firstLast(x:List) =>
        return head(x), last(x)

    pure fibonacci(n:Number) =>
        match n =>
            > 1: fibonacci(n - 2) + fibonacci(n - 1)
            0..1: 1

    pure conditionalsTest(n) =>
        if n then 42 else 2

    pure complexConditionalsTest(n) =>
        if c >= 'a' && c <= 'e'
            then 1
            else if c >= 'f' && c <= 'q'
                then 2
                else 3

    // Import in own namespace
    import math

    // Import only one method, name imported method inside this namespace
    import math.sqrt as Sq

    pure squareRootAbs(x:Double) =>
        // Namespace required here
        math.abs(math.sqrt(x))

    pure squareRootAbs2(x:Double) =>
        // Namespace required for abs, but Sq imported here
        math.abs(Sq(x))

    // Futures and concurrency
    // Everything is pure by default, so no side effects
    // IO has side effects, so it's handled with care

    // Simple calculation: get list, return two first elements summed up
    pure dosum(x:List) : List =>
        match length(x) =>
            0: []
            1: x
            2: [head(x) + at(x, 1)]
            >2: dosum(half(x)) ++ dosum(halfRest(x))

    pure parallelSum(x:List) : List =>
        match length(x) =>
            0: []
            1: x
            2: [head(x) + at(x, 1)]
            >2: par parallelSum(half(x)) ++ par parallelSum(halfRest(x))

    side printResult(x) =>
        IO.print(head(x))

    side promises() =>
        // Bind result of call to method
        bind(Promise(parallelSum, [1, 2, 3, 4]), printResult)

        prom : Promise = Promise(parallelSum, [1, 2, 3, 4])
        handlePromise(prom)

    pure lambdaCurryExample() =>
        l1 : Lambda = (val => val + 2)
        l2 : Lambda = (val, add => val + add + 1)
        // Currying l2
        l3 : Lambda = l2(2)
        l1(5) == 7
        l1(4) == 6
        l2(4, 2) == 7
        l2(3, 2) == 6
        l3(2) == 5
        // Currying second parameter
        l4 : Lambda = l2(_, 5)
        l4(2) == 8
        bind(Promise((a, b => a + b), [1, 2, 3, 4]), printResult)

    side handlePromise(p:Promise) =>
        // Bind to lambda
        bind(p, (res =>
            match res =>
                Exception: IO.print("Got error" + res)
                ?: IO.print(head(res))
        ))

    side iterTest(d:List) =>
        // Each method with lambda
        each(d, (val =>
            IO.print(val)
        ))

    pure iterTest(d:List) =>
        // Map && lazy
        Lambda add = (num, other => num + other)
        map(d, add(1))

        Lambda mapify = (a, b => ['key': a, 'value': b])
        Map data = map([['a',1],['b',2],['a',3],['d',4],['f',5],['a',6],['p',7],['q',8],['m',9]], mapify)
        at(data, 'm') == 9

    // IO

    side iotest() =>
        IO.print("Hello, world!")
        write(IO.stdout, "Hello, world!)

    side iotest2() =>
        File txt = IO.createFile("test.txt")
        write(txt, "Hello, world!)

        writeNum(txt, 42)

    side writeNum(f:File, n:Number) =>
        write(f, "Num: " + n :: String + "\n")

    side main() =>
        person = generatePerson("Eric Example", generateDate(1990, 10, 5))
        age = timeInYearsToDay(person.birthDay)
        x, y = multiReturn(42, 5)

        1, [2, 3, 4] == splitArray([1, 2, 3, 4])
        1, [3, 4] == splitArray2nd([1, 2, 3, 4])

        conditionalsTest(1)
        conditionalsTest(true)
        conditionalsTest(6 == 5)
        conditionalsTest(6 == 6)
        conditionalsTest("test")
        conditionalsTest([])

        complexConditionalsTest('a')
        complexConditionalsTest('d')
        complexConditionalsTest('f')
        complexConditionalsTest('i')
        complexConditionalsTest('r')
        complexConditionalsTest('t')
        complexConditionalsTest('z')

        [2, 3, 8, 10, 42] == map([1, 2, 7, 9, 41], add(1))

        # Float divide
        41.0 / 2 == 20.5

        # Integer division
        41 div 2 == 20
        # Integer modulus
        -41 mod 2 == 1
        # Integer remainder
        -41 rem 2 == -1

        dosum([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]) == [66]
        parallelSum([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]) == [66]

        // Squares are optional in trivial cases
        // same as: dosum(half([1, 2]))
        //dosum half [1, 2]
