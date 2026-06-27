import { useEffect, useState } from 'react'
import { getAnalytics, getLowStock, deleteJersey, updateStock, getDiscountSuggestion } from '../api/client'
import toast from 'react-hot-toast'
import { Trash2, RefreshCw, TrendingUp, Package, AlertTriangle, Sparkles, Brain } from 'lucide-react'

function StatCard({ label, value, icon, color='var(--accent)' }) {
  return (
    <div className="card" style={{ padding: 24 }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start' }}>
        <div>
          <p style={{ fontSize: '0.8rem', color: 'var(--text-secondary)', marginBottom: 8 }}>{label}</p>
          <p style={{ fontFamily: 'var(--font-display)', fontSize: '2rem', color, lineHeight: 1 }}>{value}</p>
        </div>
        <div style={{ color, opacity: 0.6 }}>{icon}</div>
      </div>
    </div>
  )
}

export default function Admin() {
  const [analytics, setAnalytics]   = useState(null)
  const [lowStock,  setLowStock]    = useState([])
  const [discounts, setDiscounts]   = useState({})   // jersey_id → AI discount suggestion
  const [discLoading, setDiscLoad]  = useState(false)
  const [tab, setTab]               = useState('overview')

  useEffect(() => {
    getAnalytics().then(r => setAnalytics(r.data.data)).catch(() => {})
    getLowStock().then(r  => setLowStock(r.data.data || [])).catch(() => {})
  }, [])

  // Fetch AI discount suggestions for low stock items
  const fetchDiscounts = async () => {
    if (lowStock.length === 0) return
    setDiscLoad(true)
    const results = {}
    await Promise.allSettled(
      lowStock.map(async j => {
        try {
          const res = await getDiscountSuggestion(j.id, j.stock, 45, true)
          results[j.id] = res.data
        } catch {}
      })
    )
    setDiscounts(results)
    setDiscLoad(false)
    toast.success('AI discount suggestions loaded!')
  }

  const handleDelete = async id => {
    if (!confirm('Delete this jersey?')) return
    await deleteJersey(id)
    toast.success('Jersey deleted')
    getLowStock().then(r => setLowStock(r.data.data || []))
  }

  const handleRestock = async id => {
    const qty = prompt('New stock quantity:')
    if (!qty) return
    await updateStock(id, parseInt(qty))
    toast.success('Stock updated!')
    getLowStock().then(r => setLowStock(r.data.data || []))
  }

  return (
    <div className="container" style={{ padding: '40px 24px' }}>
      <h2 style={{ marginBottom: 8 }}>ADMIN <span className="accent">DASHBOARD</span></h2>
      <p className="muted" style={{ marginBottom: 32, fontSize: '0.9rem' }}>Store analytics and management</p>

      {/* Tab switcher */}
      <div style={{ display: 'flex', gap: 6, marginBottom: 32, borderBottom: '1px solid var(--border)' }}>
        {['overview','lowstock','ai-insights'].map(t => (
          <button key={t} onClick={() => setTab(t)}
            style={{
              background: 'none', border: 'none', cursor: 'pointer',
              padding: '10px 20px',
              fontFamily: 'var(--font-display)', fontSize: '1rem',
              color: tab===t ? 'var(--accent)' : 'var(--text-secondary)',
              borderBottom: tab===t ? '2px solid var(--accent)' : '2px solid transparent',
              textTransform: 'uppercase', transition: 'var(--transition)',
            }}>
            {t==='ai-insights' ? '🤖 AI Insights' : t==='overview' ? 'Overview' : 'Low Stock'}
            {t==='lowstock' && lowStock.length > 0 && (
              <span style={{ marginLeft: 8, background: 'var(--red)', color: '#fff',
                fontSize: '0.65rem', padding: '2px 6px', borderRadius: 999 }}>
                {lowStock.length}
              </span>
            )}
          </button>
        ))}
      </div>

      {/* Overview tab */}
      {tab==='overview' && analytics && (
        <div className="fade-up">
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(200px,1fr))', gap: 16, marginBottom: 40 }}>
            <StatCard label="Total Revenue"  value={`₹${(analytics.totalRevenue||0).toFixed(0)}`} icon={<TrendingUp size={24}/>} />
            <StatCard label="Total Orders"   value={analytics.totalOrders}   icon={<Package size={24}/>} />
            <StatCard label="Football Sales" value={analytics.footballSales} icon={<span style={{fontSize:'1.5rem'}}>⚽</span>} color="var(--blue)" />
            <StatCard label="Cricket Sales"  value={analytics.cricketSales}  icon={<span style={{fontSize:'1.5rem'}}>🏏</span>} color="var(--gold)" />
          </div>

          <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: 24 }}>
            <div className="card" style={{ padding: 24 }}>
              <h3 style={{ marginBottom: 20, fontSize: '1rem', color: 'var(--text-secondary)', fontFamily: 'var(--font-body)', fontWeight: 600 }}>TOP SELLING JERSEYS</h3>
              {(analytics.topJerseys||[]).map((j,i) => (
                <div key={j.name} style={{ display: 'flex', justifyContent: 'space-between', padding: '10px 0', borderBottom: '1px solid var(--border)' }}>
                  <div style={{ display: 'flex', alignItems: 'center', gap: 12 }}>
                    <span style={{ fontFamily: 'var(--font-display)', fontSize: '1.2rem', color: i===0?'var(--gold)':'var(--text-muted)', minWidth: 24 }}>{i+1}</span>
                    <span style={{ fontSize: '0.875rem' }}>{j.name}</span>
                  </div>
                  <div style={{ textAlign: 'right' }}>
                    <p style={{ color: 'var(--accent)', fontFamily: 'var(--font-mono)', fontSize: '0.8rem' }}>{j.sold} sold</p>
                    <p style={{ color: 'var(--text-muted)', fontSize: '0.72rem' }}>₹{(j.revenue||0).toFixed(0)}</p>
                  </div>
                </div>
              ))}
            </div>

            <div className="card" style={{ padding: 24 }}>
              <h3 style={{ marginBottom: 20, fontSize: '1rem', color: 'var(--text-secondary)', fontFamily: 'var(--font-body)', fontWeight: 600 }}>TOP TEAMS</h3>
              {(analytics.topTeams||[]).map((t,i) => (
                <div key={t.team} style={{ display: 'flex', justifyContent: 'space-between', padding: '10px 0', borderBottom: '1px solid var(--border)' }}>
                  <div style={{ display: 'flex', alignItems: 'center', gap: 12 }}>
                    <span style={{ fontFamily: 'var(--font-display)', fontSize: '1.2rem', color: 'var(--text-muted)', minWidth: 24 }}>{i+1}</span>
                    <span style={{ fontSize: '0.875rem' }}>{t.team}</span>
                  </div>
                  <span style={{ color: 'var(--accent)', fontFamily: 'var(--font-mono)', fontSize: '0.8rem' }}>{t.sold} sold</span>
                </div>
              ))}
            </div>
          </div>
        </div>
      )}

      {/* Low Stock tab */}
      {tab==='lowstock' && (
        <div className="fade-up">
          <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: 20 }}>
            <div style={{ display: 'flex', alignItems: 'center', gap: 8 }}>
              <AlertTriangle size={18} color="var(--red)" />
              <p style={{ color: 'var(--text-secondary)', fontSize: '0.9rem' }}>
                {lowStock.length} jerseys need restocking
              </p>
            </div>
          </div>

          <div style={{ display: 'flex', flexDirection: 'column', gap: 12 }}>
            {lowStock.map(j => (
              <div key={j.id} className="card" style={{
                padding: '16px 20px', display: 'flex',
                justifyContent: 'space-between', alignItems: 'center', gap: 16,
                borderColor: j.stock < 3 ? 'rgba(255,77,77,0.3)' : 'var(--border)',
              }}>
                <div>
                  <p style={{ fontWeight: 600, marginBottom: 2 }}>{j.name}</p>
                  <p style={{ fontSize: '0.8rem', color: 'var(--text-secondary)' }}>
                    {j.team} · ₹{j.finalPrice}
                  </p>
                </div>
                <div style={{ display: 'flex', alignItems: 'center', gap: 12 }}>
                  <span style={{
                    fontFamily: 'var(--font-mono)', fontSize: '0.85rem',
                    color: j.stock < 3 ? 'var(--red)' : 'var(--gold)',
                    background: j.stock < 3 ? 'rgba(255,77,77,0.1)' : 'rgba(255,209,102,0.1)',
                    padding: '4px 12px', borderRadius: 999,
                  }}>{j.stock} left</span>
                  <button className="btn btn-ghost" onClick={() => handleRestock(j.id)} style={{ padding: '6px 12px', fontSize: '0.8rem' }}>
                    <RefreshCw size={13} /> Restock
                  </button>
                  <button className="btn btn-danger" onClick={() => handleDelete(j.id)} style={{ padding: '6px 10px' }}>
                    <Trash2 size={13} />
                  </button>
                </div>
              </div>
            ))}
          </div>
        </div>
      )}

      {/* AI Insights tab */}
      {tab==='ai-insights' && (
        <div className="fade-up">
          <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', marginBottom: 28 }}>
            <div>
              <div style={{ display: 'flex', alignItems: 'center', gap: 8, marginBottom: 4 }}>
                <Brain size={20} color="var(--accent)" />
                <h3 style={{ fontFamily: 'var(--font-display)', fontSize: '1.4rem' }}>
                  AI <span className="accent">DISCOUNT ENGINE</span>
                </h3>
              </div>
              <p className="muted" style={{ fontSize: '0.85rem' }}>
                XGBoost model suggests optimal discounts based on stock, age and demand
              </p>
            </div>
            <button className="btn btn-primary" onClick={fetchDiscounts} disabled={discLoading}>
              <Sparkles size={15} />
              {discLoading ? 'Analysing...' : 'Run AI Analysis'}
            </button>
          </div>

          {Object.keys(discounts).length === 0 ? (
            <div style={{
              padding: '48px', textAlign: 'center',
              background: 'var(--bg-card)', borderRadius: 'var(--radius-md)',
              border: '1px dashed var(--border)',
            }}>
              <Brain size={40} style={{ opacity: 0.2, marginBottom: 12 }} />
              <p className="muted">Click "Run AI Analysis" to get discount recommendations</p>
            </div>
          ) : (
            <div style={{ display: 'flex', flexDirection: 'column', gap: 12 }}>
              {lowStock.map(j => {
                const d = discounts[j.id]
                if (!d) return null
                return (
                  <div key={j.id} className="card" style={{ padding: '20px 24px' }}>
                    <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                      <div>
                        <p style={{ fontWeight: 600, marginBottom: 4 }}>{j.name}</p>
                        <p style={{ fontSize: '0.8rem', color: 'var(--text-secondary)' }}>{j.team} · {j.stock} in stock</p>
                        <p style={{ fontSize: '0.8rem', color: 'var(--text-muted)', marginTop: 4 }}>
                          💡 {d.reason}
                        </p>
                      </div>
                      <div style={{ textAlign: 'right' }}>
                        <p style={{ fontFamily: 'var(--font-display)', fontSize: '2rem', color: 'var(--accent)', lineHeight: 1 }}>
                          {d.recommended_discount?.toFixed(1)}%
                        </p>
                        <p style={{ fontSize: '0.75rem', color: 'var(--text-secondary)', marginTop: 4 }}>
                          AI recommended discount
                        </p>
                        <p style={{ fontSize: '0.75rem', color: 'var(--accent)', marginTop: 2, fontFamily: 'var(--font-mono)' }}>
                          ₹{j.price} → ₹{Math.round(j.price * (1 - (d.recommended_discount||0)/100))}
                        </p>
                      </div>
                    </div>
                  </div>
                )
              })}
            </div>
          )}
        </div>
      )}
    </div>
  )
}
