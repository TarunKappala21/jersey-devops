import { useEffect, useState, useCallback } from 'react'
import { useSearchParams } from 'react-router-dom'
import { getAllJerseys, getFootball, getCricket, searchJerseys, filterByPrice, getBatchDemand } from '../api/client'
import JerseyCard from '../components/JerseyCard'
import { Search, SlidersHorizontal, X } from 'lucide-react'

export default function Shop() {
  const [searchParams]          = useSearchParams()
  const [jerseys, setJerseys]   = useState([])
  const [demandMap, setDemand]  = useState({})
  const [loading, setLoading]   = useState(true)
  const [query, setQuery]       = useState('')
  const [sport, setSport]       = useState(searchParams.get('sport') || 'all')
  const [minPrice, setMinP]     = useState('')
  const [maxPrice, setMaxP]     = useState('')

  const fetchJerseys = useCallback(async () => {
    setLoading(true)
    try {
      let res
      if (query.trim())             res = await searchJerseys(query)
      else if (minPrice || maxPrice) res = await filterByPrice(minPrice||0, maxPrice||99999)
      else if (sport==='football')   res = await getFootball()
      else if (sport==='cricket')    res = await getCricket()
      else                           res = await getAllJerseys()

      const list = res.data.data || []
      setJerseys(list)

      // Fetch demand for all jerseys in background
      const ids = list.map(j => j.id)
      getBatchDemand(ids, 14).then(setDemand).catch(() => {})
    } catch { setJerseys([]) }
    finally  { setLoading(false) }
  }, [query, sport, minPrice, maxPrice])

  useEffect(() => { fetchJerseys() }, [sport])

  const handleSearch = e => { e.preventDefault(); fetchJerseys() }
  const clearFilters = () => { setQuery(''); setMinP(''); setMaxP(''); setSport('all') }

  return (
    <div className="container" style={{ padding: '40px 24px' }}>
      <div style={{ marginBottom: 32 }}>
        <h2 style={{ marginBottom: 8 }}>SHOP <span className="accent">JERSEYS</span></h2>
        <p className="muted" style={{ fontSize: '0.9rem' }}>{jerseys.length} jerseys found</p>
      </div>

      {/* Filters */}
      <div style={{
        background: 'var(--bg-card)', border: '1px solid var(--border)',
        borderRadius: 'var(--radius-md)', padding: '20px', marginBottom: 32,
        display: 'flex', gap: 12, flexWrap: 'wrap', alignItems: 'flex-end',
      }}>
        <form onSubmit={handleSearch} style={{ flex: 2, minWidth: 200, display: 'flex', gap: 8 }}>
          <div style={{ position: 'relative', flex: 1 }}>
            <Search size={15} style={{ position: 'absolute', left: 12, top: '50%', transform: 'translateY(-50%)', color: 'var(--text-muted)' }} />
            <input className="input" placeholder="Search player, team..." value={query}
              onChange={e => setQuery(e.target.value)} style={{ paddingLeft: 36 }} />
          </div>
          <button className="btn btn-primary" type="submit">Search</button>
        </form>

        <div style={{ display: 'flex', gap: 6 }}>
          {['all','football','cricket'].map(s => (
            <button key={s} onClick={() => setSport(s)}
              className={sport===s ? 'btn btn-primary' : 'btn btn-ghost'}
              style={{ padding: '8px 14px', fontSize: '0.8rem', textTransform: 'capitalize' }}>
              {s==='football'?'⚽':s==='cricket'?'🏏':'🏆'} {s==='all'?'All':s}
            </button>
          ))}
        </div>

        <div style={{ display: 'flex', gap: 8, alignItems: 'center' }}>
          <SlidersHorizontal size={14} color="var(--text-muted)" />
          <input className="input" placeholder="Min ₹" type="number" value={minPrice}
            onChange={e => setMinP(e.target.value)} style={{ width: 90 }} />
          <span className="muted">–</span>
          <input className="input" placeholder="Max ₹" type="number" value={maxPrice}
            onChange={e => setMaxP(e.target.value)} style={{ width: 90 }} />
          <button className="btn btn-ghost" onClick={fetchJerseys} style={{ padding: '8px 12px' }}>Apply</button>
        </div>

        <button className="btn btn-ghost" onClick={clearFilters} style={{ padding: '8px 12px' }}>
          <X size={14} /> Clear
        </button>
      </div>

      {/* Grid with demand data */}
      {loading ? (
        <div style={{ textAlign: 'center', padding: 60, color: 'var(--text-muted)' }}>Loading jerseys...</div>
      ) : jerseys.length === 0 ? (
        <div className="empty-state">
          <span style={{ fontSize: '3rem' }}>🔍</span>
          <p>No jerseys found</p>
          <button className="btn btn-ghost" onClick={clearFilters}>Clear filters</button>
        </div>
      ) : (
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(260px,1fr))', gap: 20 }}>
          {jerseys.map(j => (
            <JerseyCard
              key={j.id}
              jersey={j}
              demand={demandMap[j.id]}
            />
          ))}
        </div>
      )}
    </div>
  )
}
