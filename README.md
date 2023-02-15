# HTTP_Caching_Proxy
Requirements
-- 
1. support GET
2. support POST
3. support CONNECT
4. You should follow rules of expiration time
5. You should follow rules of re-validation
6. Please specify your cache policy (e.g. cache size, replacement policy, when to clean expired reponse) 
7. Do you spawn a thread/process to handle a request?
8. Did do you appropriately handle necessary synchronization for your cache (note a simple strategy like "using a mutex to lock to protect the entire cache for every operation" might not be the prefect answer from our perspective)
9. You should have a log file to record required information
10. You should organize your code in good C++ style. 
11. Your code should be robust to external failures. (i.e. What failures did you handle in your code? how do you handle them? You can answer these in your danger log.)
12. You should know the exception guarantees you make (Please specify them (within your danger log).
13. You should setup docker for submission, per the requirement
14. You should provide testcases by the requirment!
