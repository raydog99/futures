use std::num::NonZeroUsize;
use std::sync::{Arc, Mutex};
use async_trait::async_trait;
use lru::LruCache;
use tokio::sync::Mutex as TokioMutex;

const CLIENT_CACHE_SIZE: usize = 20;

#[async_trait]
pub trait Connect: Clone + Send + Sync + 'static {
    type Error;
    async fn connect(endpoint: String) -> Result<Self, Self::Error>;
}

#[derive(Debug, Clone)]
pub struct ClientPool<T> {
    client_cache: Arc<TokioMutex<LruCache<String, T>>>,
}

impl<T> ClientPool<T>
where
    T: Connect,
{
    pub fn new() -> Self {
        Self {
            client_cache: Arc::new(TokioMutex::new(LruCache::new(
                NonZeroUsize::new(CLIENT_CACHE_SIZE).unwrap(),
            ))),
        }
    }

    pub async fn connect(&self, address: &str) -> Result<T, T::Error> {
        let client = T::connect(address.to_string()).await?;
        self.client_cache
            .lock()
            .await
            .put(address.to_string(), client.clone());
        Ok(client)
    }

    pub async fn get_client(&self, address: &str) -> Result<T, T::Error> {
        if let Some(client) = self.client_cache.lock().await.get(address) {
            return Ok(client.clone());
        }
        self.connect(address).await
    }
}