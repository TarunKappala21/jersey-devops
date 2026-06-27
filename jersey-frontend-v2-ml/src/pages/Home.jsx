import { useEffect, useState } from 'react'
import { Link } from 'react-router-dom'
import { getAllJerseys, getMLRecommendations, getBatchDemand } from '../api/client'
import { useAuth } from '../context/AuthContext'
import JerseyCard from '../components/JerseyCard'
import { ArrowRight, Zap, Sparkles } from 'lucide-react'

function SkeletonCard() {
  return (
    <div style={{ background: 'var(--bg-card)', borderRadius: 'var(--radius-md)', overflow: 'hidden' }}>
      <div className="skeleton" style={{ height: 4 }} />
      <div style={{ padding: 20, display: 'flex', flexDirection: 'column', gap: 12 }}>
        <div className="skeleton" style={{ height: 20, width: '60%' }} />
        <div className="skeleton" style={{ height: 28, width: '80%' }} />
        <div className="skeleton" style={{ height: 16 }} />
        <div className="skeleton" style={{ height: 36, marginTop: 8 }} />
      </div>
    </div>
  )
}

export default function Home() {
  const { user }               = useAuth()
  const [jerseys, setJerseys]  = useState([])
  const [recs, setRecs]        = useState([])       // ML recommendations
  const [demandMap, setDemand] = useState({})       // ML demand per jersey
  const [loading, setLoading]  = useState(true)
  const [recLoading, setRecLoading] = useState(true)

  useEffect(() => {
    // Fetch all jerseys
    getAllJerseys()
      .then(r => {
        const all = r.data.data || []
        setJerseys(all)
        // Fetch batch demand for featured jerseys (first 4)
        const ids = all.slice(0,6).map(j => j.id)
        getBatchDemand(ids, 14)
          .then(setDemand)
          .catch(() => {})
      })
      .catch(() => {})
      .finally(() => setLoading(false))

    // ML Recommendations
    if (user && user.role !== 'admin') {
      const fanId = user.fanId || 101
      getMLRecommendations(fanId)
        .then(r => setRecs(r.data?.recommended || []))
        .catch(() => {})
        .finally(() => setRecLoading(false))
    } else {
      setRecLoading(false)
    }
  }, [user])

  // Convert ML rec format to jersey card format
  const mlRecToJersey = (rec) => ({
    id: rec.id, name: rec.name, team: rec.team,
    sport: rec.sport, price: rec.price,
    finalPrice: rec.price, discount: 0,
    stock: 10, inStock: true, season: '2024/25',
    details: rec.team, limited: false,
    player: rec.name,
  })

  const featured = jerseys.slice(0, 4)

  return (
    <div>
      {/* ── Hero ── */}
      <div style={{
        background: `
          radial-gradient(ellipse at 70% 50%, rgba(181,255,77,0.08) 0%, transparent 60%),
          radial-gradient(ellipse at 20% 80%, rgba(77,159,255,0.06) 0%, transparent 50%)
        `,
        borderBottom: '1px solid var(--border)',
        padding: '80px 24px',
      }}>
        <div className="container">
          <div style={{ maxWidth: 640 }}>
            <div style={{
              display: 'inline-flex', alignItems: 'center', gap: 8,
              background: 'var(--accent-glow)', border: '1px solid var(--border-accent)',
              borderRadius: 999, padding: '4px 14px', marginBottom: 24,
              fontSize: '0.78rem', fontWeight: 600, color: 'var(--accent)',
            }}>
              <Zap size={12} /> NEW SEASON COLLECTION
            </div>
            <h1 style={{ marginBottom: 20 }}>
              WEAR YOUR<br /><span className="accent">PASSION.</span>
            </h1>
            <p style={{ fontSize: '1.1rem', color: 'var(--text-secondary)', marginBottom: 32, maxWidth: 480 }}>
              Authentic football & cricket jerseys. Personalised picks just for you.
            </p>
            <div style={{ display: 'flex', gap: 12, flexWrap: 'wrap' }}>
              <Link to="/jerseys" className="btn btn-primary" style={{ padding: '12px 28px', fontSize: '1rem' }}>
                Shop Now <ArrowRight size={16} />
              </Link>
              <Link to="/jerseys?sport=cricket"  className="btn btn-ghost" style={{ padding: '12px 28px' }}>🏏 Cricket</Link>
              <Link to="/jerseys?sport=football" className="btn btn-ghost" style={{ padding: '12px 28px' }}>⚽ Football</Link>
            </div>
          </div>
        </div>
      </div>

      <div className="container" style={{ padding: '60px 24px' }}>

        {/* ── Stats ── */}
        <div style={{ display: 'grid', gridTemplateColumns: 'repeat(3,1fr)', gap: 16, marginBottom: 64 }}>
          {[
            { label: 'Jerseys',  value: jerseys.length + '+' },
            { label: 'Clubs',    value: '10+' },
            { label: 'Sports',   value: '2' },
          ].map(s => (
            <div key={s.label} className="card" style={{ padding: 24, textAlign: 'center' }}>
              <p style={{ fontFamily: 'var(--font-display)', fontSize: '2.5rem', color: 'var(--accent)', lineHeight: 1 }}>{s.value}</p>
              <p style={{ color: 'var(--text-secondary)', fontSize: '0.85rem', marginTop: 4 }}>{s.label}</p>
            </div>
          ))}
        </div>

        {/* ── ML Recommendations ── */}
        {user && user.role !== 'admin' && (
          <section style={{ marginBottom: 64 }}>
            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'baseline', marginBottom: 24 }}>
              <div>
                <div style={{ display: 'flex', alignItems: 'center', gap: 8, marginBottom: 4 }}>
                  <Sparkles size={18} color="var(--accent)" />
                  <h2>PICKED <span className="accent">FOR YOU</span></h2>
                </div>
                <p className="muted" style={{ fontSize: '0.82rem' }}>
                  Personalised by our recommendation engine
                </p>
              </div>
              <Link to="/jerseys" style={{ color: 'var(--text-secondary)', fontSize: '0.85rem', textDecoration: 'none' }}>
                See all →
              </Link>
            </div>

            {recLoading ? (
              <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(260px,1fr))', gap: 20 }}>
                {Array(3).fill(0).map((_,i) => <SkeletonCard key={i} />)}
              </div>
            ) : recs.length > 0 ? (
              <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(260px,1fr))', gap: 20 }}>
                {recs.slice(0,4).map(rec => (
                  <JerseyCard
                    key={rec.id}
                    jersey={mlRecToJersey(rec)}
                    demand={demandMap[rec.id]}
                  />
                ))}
              </div>
            ) : (
              <div style={{
                padding: '32px', textAlign: 'center',
                background: 'var(--bg-card)', borderRadius: 'var(--radius-md)',
                border: '1px solid var(--border)', color: 'var(--text-muted)',
              }}>
                <p>Browse some jerseys to get personalised picks ✨</p>
              </div>
            )}
          </section>
        )}

        {/* ── Featured ── */}
        <section>
          <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'baseline', marginBottom: 24 }}>
            <h2>FEATURED <span className="accent">JERSEYS</span></h2>
            <Link to="/jerseys" style={{ color: 'var(--text-secondary)', fontSize: '0.85rem', textDecoration: 'none' }}>
              View all →
            </Link>
          </div>
          <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(260px,1fr))', gap: 20 }}>
            {loading
              ? Array(4).fill(0).map((_,i) => <SkeletonCard key={i} />)
              : featured.map(j => (
                  <JerseyCard
                    key={j.id}
                    jersey={j}
                    demand={demandMap[j.id]}
                  />
                ))
            }
          </div>
        </section>
      </div>
    </div>
  )
}
