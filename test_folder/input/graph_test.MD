### Test Case 1: Simple Linear Graph
```mermaid
graph LR;
    X-->Y;
    Y-->Z;
```

### Test Case 2: Branching Graph
```mermaid
graph TD;
    E-->F;
    E-->G;
    E-->H;
```

### Test Case 3: Complex Graph with Multiple Connections
```mermaid
graph TD;
    I-->J;
    I-->K;
    J-->L;
    K-->L;
    L-->M;
    J-->M;
```

### Test Case 4: Cyclic Graph
```mermaid
graph TD;
    N-->O;
    O-->P;
    P-->N;
```

### Test Case 5: Larger Acyclic Graph
```mermaid
graph TD;
    Q-->R;
    Q-->S;
    R-->T;
    S-->T;
    T-->U;
    S-->V;
    U-->W;
    V-->W;
    V-->X;
```

### Test Case 6: Deep Tree Structure
```mermaid
graph TD;
    A-->B;
    B-->C;
    C-->D;
    D-->E;
```

### Test Case 7: Wide Tree Structure
```mermaid
graph TD;
    A-->B;
    A-->C;
    A-->D;
    A-->E;
    A-->F;
```
