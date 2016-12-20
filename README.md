#### Intersect_sets
Пересечение двух групп множеств

#### Пример

``` 
./intersect_sets data/A data/B >> intersection.txt
```

Результат

```
intersections.txt
A/01.txt	100
A/02.txt	100
A/03.txt	100
B/02.txt	100
B/03.txt	100
B/01.txt	100
A/01.txt-B/01.txt	50
A/01.txt-B/02.txt	20
A/01.txt-B/03.txt	14
A/02.txt-B/01.txt	33
A/02.txt-B/02.txt	20
A/02.txt-B/03.txt	14
A/03.txt-B/01.txt	14
A/03.txt-B/02.txt	14
A/03.txt-B/03.txt	100
```