import asyncio
from typing import Generic, TypeVar
from abc import ABC, abstractmethod
from cachetools import LRUCache

T = TypeVar('T')

CLIENT_CACHE_SIZE = 20

class Connect(ABC, Generic[T]):
    @abstractmethod
    async def connect(self, endpoint: str) -> T:
        pass

class ClientPool(Generic[T]):
    def __init__(self):
        self.client_cache = LRUCache(maxsize=CLIENT_CACHE_SIZE)
        self.lock = asyncio.Lock()

    async def connect(self, address: str, connector: Connect[T]) -> T:
        client = await connector.connect(address)
        async with self.lock:
            self.client_cache[address] = client
        return client

    async def get_client(self, address: str, connector: Connect[T]) -> T:
        async with self.lock:
            if address in self.client_cache:
                return self.client_cache[address]
        return await self.connect(address, connector)