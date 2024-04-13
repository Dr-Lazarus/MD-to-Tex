### Bello world

### Test Case 1: Simple Sequence Diagram
```mermaid
sequenceDiagram
    participant Alice
    participant Bob
    Alice->>Bob: Hi Bob
    Bob->>Alice: Hi Alice
```

### Test Case 2: Sequence Diagram With More Participants
```mermaid
sequenceDiagram
    participant Alice
    participant Bob
    participant Charlie
    Alice->>Bob: Hi Bob
    Bob->>Charlie: Hi Charlie
    Charlie->>Alice: Hi Alice
    Alice->>Alice: I like talking to myself
    Bob->>Bob: Me too!
```

### Test Case 3: Sequence Diagram With Implicit Participant Declaration
```mermaid
sequenceDiagram
    participant Bob Charger
    Alice Wex->>Bob Charger: Hi Bob Charger
    Bob Charger->>Charlie Dugg: Hi Charlie Dugg
    Charlie Dugg->>Alice Wex: Hi Alice Wex
```

### Test Case 4: Prev test but with random white spaces in mermaid
```mermaid
sequenceDiagram    
        participant   Bob Charger     
    Alice Wex       ->>  Bob Charger    :    Hi Bob Charger        
 Bob Charger ->> Charlie Dugg           :        Hi Charlie Dugg
         Charlie Dugg              ->>Alice Wex:    Hi Alice Wex 
```

### Test Case 5: Empty sequenceDiagram
```mermaid
sequenceDiagram
```

### Test Case 6: Redeclare Participants
```mermaid
sequenceDiagram
    participant Alice One
    participant Alice Two
    Alice One->>Bob: Hi Bob
    Bob->>Charlie: Hi Charlie
    Charlie->>Alice Two: Hi Alice Two
    participant Bob
    participant Charlie
    participant Charlie
```