from dataclasses import dataclass
from typing import Generic, TypeVar

T = TypeVar('T')

@dataclass
class DFut(Generic[T]):
    d_store_id: 'DStoreId'

    @classmethod
    def new(cls, d_store_id: 'DStoreId') -> 'DFut[T]':
        return cls(d_store_id)

    def share(self) -> 'DFut[T]':
        return DFut(self.d_store_id.clone())

    @classmethod
    def from_d_store_id(cls, d_store_id: 'DStoreId') -> 'DFut[T]':
        return cls.new(d_store_id)