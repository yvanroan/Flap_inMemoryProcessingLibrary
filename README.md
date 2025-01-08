# Flap InMemory Processing Library

Flap aims to be a high-performance in-memory data processing library that dynamically adapts between row-based and columnar storage.
It enables efficient querying and manipulation of datasets by choosing the optimal storage and processing strategy based on workload patterns.
This hybrid approach targets both transactional and analytical tasks, filling the gap left by existing row-based (e.g., Pandas) and columnar (e.g., Arrow) libraries.

I know that it wouldn't be memory efficient to switch from one form to the other but i thought about it and wanted to do it :-) 


## Layer Responsibilities(current setup)

### Data Layer:

  **Array**:
    Represents columnar data with optional and variant types.
    Provides operations like filter, map, append, and aggregate.
    
  **Table**:
    Represents tabular data composed of multiple Arrays.
    Supports operations like appendCol, appendRow, join, and aggregation.
    
### API Layer:

  Provides user-facing methods (e.g., table.appendCol, array.filter).
  Translates method calls into structured inputs for the execution layer.
  Passes the parsed operations (QueryNodes) to the execution layer.
  
### Memory Layer:

  **MemoryManager**:
    Tracks memory usage for objects (Array, Table).
    Implements an LFU cache for efficient memory usage.
    Handles memory thresholds by triggering data spilling.
  **DiskSpiller**:
    Serializes and stores evicted objects on disk.
    Deserializes and reloads objects into memory as needed.
  
### Execution Layer:

  **QueryParser**:
    Parses user inputs into structured QueryNodes. Validates parameters and ensures input correctness.
    
  **QueryPlanner/Executioner**:
    Manages query plans by adding, validating, and executing QueryNodes. Ensures memory availability for operations by interfacing with MemoryManager and DiskSpiller.
    Dynamically executes operations on Array or Table objects.
