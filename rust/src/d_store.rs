use std::sync::Arc;

#[derive(Debug, Clone)]
pub struct DStoreId(Arc<u64>);

impl DStoreId {
    pub fn new(id: u64) -> Self {
        DStoreId(Arc::new(id))
    }

    pub fn clone(&self) -> Self {
        DStoreId(Arc::clone(&self.0))
    }
}