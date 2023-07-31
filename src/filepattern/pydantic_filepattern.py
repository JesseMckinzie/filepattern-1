from pydantic import BaseModel
from typing import Optional

class FilePatternInitializer(BaseModel):
    
    path: str
    pattern: Optional[str] = ""
    block_size: Optional[str] = ""
    recursive: Optional[bool] = False
    suppress_warnings: Optional[bool] = False


class GetMatching(BaseModel):
    variables: dict
    
    
    
class GroupBy(BaseModel):
    group_by: dict