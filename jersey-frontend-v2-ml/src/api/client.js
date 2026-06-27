import axios from 'axios'

const BASE    = import.meta.env.VITE_API_URL    || 'http://localhost:8080'
const ML_BASE = import.meta.env.VITE_ML_API_URL || 'http://localhost:8001'

const api    = axios.create({ baseURL: BASE })
const mlApi  = axios.create({ baseURL: ML_BASE })

api.interceptors.request.use(cfg => {
  const token = localStorage.getItem('token')
  if (token) cfg.headers['Authorization'] = token
  return cfg
})

// ── C++ API ───────────────────────────────────
export const login           = (username, password) => api.post('/auth/login',    { username, password })
export const register        = (username, password, name) => api.post('/auth/register', { username, password, name })
export const getAllJerseys    = ()          => api.get('/jerseys')
export const getFootball     = ()          => api.get('/jerseys/football')
export const getCricket      = ()          => api.get('/jerseys/cricket')
export const searchJerseys   = (q)         => api.get(`/jerseys/search?q=${encodeURIComponent(q)}`)
export const filterByPrice   = (min, max)  => api.get(`/jerseys/filter?min=${min}&max=${max}`)
export const getJersey       = (id)        => api.get(`/jerseys/${id}`)
export const addJersey       = (data)      => api.post('/jerseys', data)
export const updateStock     = (id, stock) => api.put(`/jerseys/${id}/stock`, { stock })
export const deleteJersey    = (id)        => api.delete(`/jerseys/${id}`)
export const getCart         = (fanId)     => api.get(`/cart/${fanId}`)
export const addToCart       = (fanId, jerseyId, quantity) => api.post(`/cart/${fanId}/add`, { jerseyId, quantity })
export const removeFromCart  = (fanId, jerseyId) => api.delete(`/cart/${fanId}/${jerseyId}`)
export const checkout        = (fanId)     => api.post(`/cart/${fanId}/checkout`)
export const getAnalytics    = ()          => api.get('/admin/analytics')
export const getLowStock     = ()          => api.get('/admin/lowstock')

// ── ML API ────────────────────────────────────
export const getMLRecommendations = (fanId, boughtIds = []) =>
  mlApi.get(`/recommend/${fanId}?top_n=6&already_bought=${boughtIds.join(',')}`)

export const getDemandForecast = (jerseyId, daysToEvent = 14) =>
  mlApi.post('/demand', { jersey_id: jerseyId, days_to_event: daysToEvent })

export const getDiscountSuggestion = (jerseyId, stockLevel, daysSinceLaunch, seasonActive = true) =>
  mlApi.post('/discount', {
    jersey_id: jerseyId,
    stock_level: stockLevel,
    days_since_launch: daysSinceLaunch,
    season_active: seasonActive,
  })

// Batch demand for multiple jerseys at once
export const getBatchDemand = async (jerseyIds, daysToEvent = 14) => {
  const results = await Promise.allSettled(
    jerseyIds.map(id => getDemandForecast(id, daysToEvent))
  )
  const map = {}
  results.forEach((r, i) => {
    if (r.status === 'fulfilled')
      map[jerseyIds[i]] = r.value.data
  })
  return map
}

export default api
