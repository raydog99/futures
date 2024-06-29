use std::marker::PhantomData;
use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize, Deserialize)]
pub struct DFut<T> {
    pub(crate) d_store_id: DStoreId,
    #[serde(skip)]
    phantom_data: PhantomData<T>,
}

impl<T> DFut<T> {
    pub(crate) fn new(d_store_id: DStoreId) -> Self {
        Self {
            d_store_id,
            phantom_data: PhantomData::default(),
        }
    }

    pub(crate) fn share(&self) -> Self {
        Self {
            d_store_id: self.d_store_id.clone(),
            phantom_data: PhantomData::default(),
        }
    }
}

impl<T> From<DStoreId> for DFut<T> {
    fn from(value: DStoreId) -> Self {
        DFut::new(value)
    }
}