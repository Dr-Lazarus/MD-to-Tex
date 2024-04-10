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