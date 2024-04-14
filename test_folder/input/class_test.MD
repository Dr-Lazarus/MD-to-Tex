### Test Case 1: Basic Single Class with No Relationships
```mermaid
classDiagram
    class User {
      +String name
      +String email
      +placeOrder()
    }
```


### Test Case 2: Association Relationship
```mermaid
classDiagram
    User "1" -- "0..*" Order : places

    class User {
      +String name
      +String email
      +placeOrder()
    }
    class Order {
      +Date orderDate
      +String status
    }
```

### Test Case 2: Aggregation Relationship
```mermaid
classDiagram
    User "1" -- "0..*" Order : has

    class User {
      +String name
      +String email
      +placeOrder()
    }
    class Order {
      +Date orderDate
      +String status
    }
```


### Test Case 3: Composition Relationship
```mermaid
classDiagram
    ShoppingCart "1" -- "0..*" Item : contains

    class ShoppingCart {
      +List<Item> items
    }
    class Item {
      +String name
      +float price
    }
```


### Test Case 4: Inheritance
```mermaid
classDiagram
    Employee "1" -- "0..*" Person : inherits

    class Person {
      +String name
      +Date birthDate
    }
    class Employee {
      +String department
    }
```


### Test Case 5: Complex Diagram with Multiple Relationships
```mermaid
classDiagram
    User "1" -- "0..*" Order : places
    Order "1" -- "1..*" Product : contains

    class User {
      +String name
      +String email
    }
    class Product {
      +String name
      +float price
    }
    class Order {
      +Date orderDate
    }
```


### Test Case 6: Complex Diagram with Multiple Relationships
```mermaid
classDiagram
    User "1" -- "0..*" Order : places
    Order "1" -- "1..*" Product : contains

    class User{
      +String name
      +String email
      +placeOrder()
    }
    class Order{
      +Date orderDate
      +String status
    }
    class Product{
      +String name
      +float price
    }
```
